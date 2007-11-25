using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace server
{
    public class CServer
    {
        private Socket m_Socket;
        public AsyncCallback pfnWorkerCallBack;
        public CConfig config;
        public int m_Port;
        public int m_ClientCount;
        public int m_PacketSize;
        public ArrayList m_ClientList;
        public CPacketBuilder m_Packet;
        public CServerPackets Packets;
        public CDatabase DB;
        public int[] ClientIDList;
        public Thread MapThread;
        public Thread WorldThread;
        public Thread VisionThread;
        public CProcess Process;
        public CItemList[] ItemList;
        public int NumLoadedItems;

        public CServer()
        {
            ItemList = new CItemList[5338];
            ClientIDList = new int[65535];
            for (int i = 0; i < 65535; i++)
                ClientIDList[i] = 0;    
            m_ClientList = ArrayList.Synchronized(new ArrayList());
            m_ClientCount = 0;
            m_Packet = new CPacketBuilder();
            config = new CConfig();
            config.LoadConfigs();
            m_Port = config.Port;
            DB = new CDatabase(config.DSN);
            Packets = new CServerPackets(this);
            Process = new CProcess(this);
            MapThread = new Thread(Process.MapProcess);
            WorldThread = new Thread(Process.WorldProcess);
            VisionThread = new Thread(Process.VisionProcess);
            MapThread.Start();
            Core.CLog("Started Map Thread");
            WorldThread.Start();
            Core.CLog("Started World Thread");
            VisionThread.Start();
            Core.CLog("Started Vision Thread");
            CLoadData.LoadItemList(this);
        }

        public void Start()
        {
            try
            {
                m_Socket = new Socket(AddressFamily.InterNetwork,
                    SocketType.Stream,
                    ProtocolType.Tcp);
                IPEndPoint ipLocal = new IPEndPoint(IPAddress.Any, m_Port);

                m_Socket.Bind(ipLocal);
                m_Socket.Listen(100);

                IPHostEntry iphe = Dns.GetHostEntry(Dns.GetHostName());
                Core.CLog(String.Format("Listening on Address: {0}:{1}", IPAddress.Loopback, m_Port));
                IPAddress[] ip = iphe.AddressList;
                for (int i = 0; i < ip.Length; ++i)
                    Core.CLog(String.Format("Listening on Address: {0}:{1}", ip[i], m_Port));

                m_Socket.BeginAccept(new AsyncCallback(OnClientConnect), null);
            }
            catch (SocketException se)
            {
                Core.CLog(se.Message);
            }
        }

        public void OnClientConnect(IAsyncResult asyn)
        {
            CClient thisclient = new CClient(this);
            CPlayer player;
            try
            {
                thisclient.sock = m_Socket.EndAccept(asyn);
                m_ClientCount++;
                m_ClientList.Add(thisclient);
                player = new CPlayer(thisclient);
                thisclient.Player = player;
                WaitForData(thisclient);
                Core.CLog(String.Format("Client Connected {0}", thisclient.sock.RemoteEndPoint.ToString()));
                m_Socket.BeginAccept(new AsyncCallback(OnClientConnect), null);
            }
            catch (SocketException se)
            {
                Core.CLog(se.Message);
            }
        }

        public void WaitForData(CClient client)
        {
            try
            {
                if (pfnWorkerCallBack == null)
                {
                    pfnWorkerCallBack = new AsyncCallback(OnDataReceived);
                }
                CPacket thispacket = new CPacket(client);
                client.sock.BeginReceive(thispacket.dataBuffer, 0, thispacket.dataBuffer.Length, SocketFlags.None, pfnWorkerCallBack, thispacket);
            }
            catch (SocketException se)
            {
                Core.CLog(se.Message);
            }
        }

        public void OnDataReceived(IAsyncResult asyn)
        {
            CClient thisclient = null;
            CPacket thispacket = (CPacket)asyn.AsyncState;
            try
            {
                m_PacketSize = thispacket.m_thisClient.sock.EndReceive(asyn);
                thisclient = thispacket.m_thisClient;
                thisclient.PacketSize = m_PacketSize;
                // Packets
                if (m_PacketSize > 0)
                {
                    PacketHandler(thisclient, thispacket);
                }
                else
                {
                    thisclient.sock.Disconnect(false);
                }

                if(thispacket.m_thisClient.sock.Connected)
                    WaitForData(thisclient);
            }
            catch (SocketException se)
            {
                if (se.ErrorCode == 10054)
                {
                    // Client Disconnect
                    //Packets.pak_PlayerDisconnect(thispacket.dataBuffer, thisclient.Player);
                    Core.CLog(String.Format("Client Disconnected {0}", thispacket.m_thisClient.sock.RemoteEndPoint.ToString()));
                }
                else
                {
                    Core.CLog(se.Message);
                }
            }
        }

        void CloseSockets()
        {
            if (m_Socket != null)
            {
                m_Socket.Close();
            }
            for (int i = 0; i < m_ClientList.Count; i++)
            {
                CClient thisclient = (CClient)m_ClientList[i];
                if (thisclient.sock != null)
                {
                    thisclient.sock.Close();
                    thisclient.sock = null;
                }
            }
        }

        public void PacketHandler(CClient thisclient, CPacket thispacket)
        {
            if (thispacket.dataBuffer[0] + thispacket.dataBuffer[1] == 0x74)
            {
                Packets.pak_AccountLogin(thispacket.dataBuffer, thisclient.Player);
            }
            else if (thispacket.dataBuffer[4] + thispacket.dataBuffer[5] == 0x74)
            {
                Packets.pak_AccountLogin(thispacket.dataBuffer, thisclient.Player);
            }
            else
            {
                if (thispacket.dataBuffer[0] + thispacket.dataBuffer[1] != 0x0104)
                {
                    byte[] psize = new byte[2];
                    int lastbyte = 0;
                    int decsize = 0;
                    while (thisclient.PacketSize != lastbyte)
                    {
                        Buffer.BlockCopy(thispacket.dataBuffer, lastbyte, psize, 0, 2);
                        int paksize = BitConverter.ToInt16(psize, 0);
                        byte[] buff = new byte[paksize];
                        byte[] src = new byte[paksize];
                        Buffer.BlockCopy(thispacket.dataBuffer, lastbyte, src, 0, paksize);
                        decsize = thisclient.encdec.Decrypt(buff, src, paksize);
                        lastbyte = lastbyte + paksize;
                        Packets.PacketOpcodeControl(buff, thisclient.Player, paksize);
                    }
                }
            }
        }

        public int GetClientID()
        {
            for (int i = 1; i < 65535; i++)
            {
                if (ClientIDList[i] != 1)
                {
                    ClientIDList[i] = 1;
                    return i;
                }
            }
            return 0;
        }

        public int Distance(fPoint p1, fPoint p2)
        {
            int distance = 0;
            int dx = p1.x - p2.x;
            int dy = p1.y - p2.y;
            distance = Convert.ToInt32(Math.Sqrt((dx * dx) + (dy * dy)));
            return distance;
        }

        public bool VisionList(CPlayer thisclient)
        {
            foreach (object client in m_ClientList)
            {
                CClient otherclient = (CClient)client;
                int distance = Distance(thisclient.Position.pCurrent, otherclient.Player.Position.pCurrent);
                if (CVisionFunc.isVisible(thisclient, otherclient.Player))
                {
                    if (distance < 25)
                    {
                        thisclient.vPlayers.Add(otherclient.Player);
                    }
                    else
                    {
                        thisclient.vPlayers.Remove(otherclient.Player);
                        Packets.pak_DeleteCharSpawn(thisclient, otherclient.Player);
                    }
                }
                else
                {
                    if (distance < 5)
                    {
                        thisclient.vPlayers.Add(otherclient.Player);
                        Packets.pak_SpawnChar(thisclient, otherclient.Player);
                    }
                }
            }
            return true;
        }
    }
}

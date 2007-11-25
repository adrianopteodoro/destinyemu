using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;

namespace server
{
    public class CClient
    {
        public Socket sock;
        public CEncryption encdec;
        public CPlayer Player;
        public CServer Server;
        public int PacketSize;
        public int Hash;
        public byte[] lkeys;

        public CClient(CServer sServer)
        {
            Server = sServer;
            lkeys = new byte[16];
            this.encdec = new CEncryption();
        }

        public void SendPacket(CPacketBuilder thispacket, int size)
        {
            if (this.sock.Connected)
            {
                byte[] SendData = new byte[size];
                Buffer.BlockCopy(thispacket.dataBuffer, 0, SendData, 0, size);
                this.sock.Send(SendData, size, SocketFlags.None);
            }
        }
    }
}

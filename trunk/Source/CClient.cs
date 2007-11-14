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
        public ulong Hash;
        public byte[] lkeys;

        public CClient(CServer sServer)
        {
            Server = sServer;
            lkeys = new byte[16];
            this.encdec = new CEncryption();
        }

        public void SendPacket(CPacketBuilder thispacket, int size)
        {
            this.sock.Send(thispacket.dataBuffer, size, SocketFlags.None);
        }
    }
}

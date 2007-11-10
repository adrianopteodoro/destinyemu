using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;

namespace server
{
    public class CPacket
    {
        public CClient m_thisClient;
        public byte[] dataBuffer;

        public CPacket(CClient thisclient)
        {
            dataBuffer = new byte[65535];
            m_thisClient = thisclient;
        }
    }

    public class CPacketBuilder
    {
        public byte[] dataBuffer;

        public CPacketBuilder()
        {
            dataBuffer = new byte[65535];
        }

        public void SetString(string value, int offset)
        {
            byte[] src = Encoding.ASCII.GetBytes(value);
            Buffer.BlockCopy(src, 0, dataBuffer, offset, value.Length);
        }

        public void SetByte(int value, int offset)
        {
            byte[] val = BitConverter.GetBytes(value);
            Buffer.BlockCopy(val, 0, dataBuffer, offset, 1);
        }

        public void SetShort(int value, int offset)
        {
            byte[] val = BitConverter.GetBytes(value);
            Buffer.BlockCopy(val, 0, dataBuffer, offset, 2);
        }

        public void SetLong(int value, int offset)
        {
            byte[] val = BitConverter.GetBytes(value);
            Buffer.BlockCopy(val, 0, dataBuffer, offset, 4);
        }

        public void Clear()
        {
            for (int i = 0; i < 65535; i++)
            {
                dataBuffer[i] = 0x00;
            }
        }
    }
}

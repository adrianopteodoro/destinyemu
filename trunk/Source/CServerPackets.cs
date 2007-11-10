using System;
using System.Collections.Generic;
using System.Text;

namespace server
{
    public class CServerPackets
    {
        public CServer mServer;
        public CPacketBuilder OutPak;

        public CServerPackets(CServer sServer)
        {
            mServer = sServer;
            OutPak = new CPacketBuilder();
        }

        public void PacketOpcodeControl(byte[] pak, CPlayer thisclient, int packetsize)
        {
            byte[] topcode = new byte[2];
            Buffer.BlockCopy(pak, 4, topcode, 0, 2);
            int opcode = BitConverter.ToInt16(topcode, 0);
            switch (opcode)
            {
                default: Core.CLog(String.Format("Received OPCODE: 0x{0:X4}", opcode)); break;
            }
        }

        public void pak_AccountLogin(byte[] pak, CPlayer thisclient)
        {
            byte[] buff = new byte[thisclient.Client.PacketSize];
            byte[] ClientVersion = new byte[2];
            if(pak[0] + pak[1] == 0x74)
            {
                thisclient.Client.encdec.Decrypt(buff, pak, 116);
                Buffer.BlockCopy(buff, 48, thisclient.Client.lkeys, 0, 16);
                thisclient.Session.username = Encoding.UTF8.GetString(buff, 12, 12).Replace("\x00", "");
                thisclient.Session.password = Encoding.UTF8.GetString(buff, 28, 12).Replace("\x00", "");
                Buffer.BlockCopy(buff, 40, ClientVersion, 0, 2);
            }
            else if(pak[4] + pak[5] == 0x74)
            {
                byte[] pakbuf = new byte[thisclient.Client.PacketSize-4];
                Buffer.BlockCopy(pak, 4, pakbuf, 0, thisclient.Client.PacketSize - 4);
                thisclient.Client.encdec.Decrypt(buff, pakbuf, 116);
                Buffer.BlockCopy(buff, 48, thisclient.Client.lkeys, 0, 16);
                thisclient.Session.username = Encoding.Default.GetString(buff, 12, 12).Replace("\x00", "");
                thisclient.Session.password = Encoding.Default.GetString(buff, 28, 12).Replace("\x00", "");
                Buffer.BlockCopy(buff, 40, ClientVersion, 0, 2);
            }
            thisclient.Client.Hash = thisclient.Client.encdec.GetHash(thisclient.Client.lkeys);
            int cliver = BitConverter.ToInt16(ClientVersion, 0);
            Core.CLog(String.Format("User Logging UID: [{0}] ClientVersion: [{1}]", thisclient.Session.username.ToString(), cliver));

            if (cliver >= mServer.config.CliVersion)
            {
                mServer.DB.QStore(String.Format("CALL sp_CheckUserID('{0}');", thisclient.Session.username));
                mServer.DB.result.Read();
                if (mServer.DB.result.GetInt32(0) == 0)
                {
                    pak_SendServerMsg("Account Name is Invalid", thisclient);
                    mServer.DB.FreeQuery();
                    thisclient.Client.sock.Close();
                }
                else
                {
                    mServer.DB.FreeQuery();
                    mServer.DB.QStore(String.Format("CALL sp_CheckUserAndPWD('{0}','{1}');", thisclient.Session.username, thisclient.Session.password));
                    mServer.DB.result.Read();
                    if (mServer.DB.result.GetInt32(0) == 0)
                    {
                        pak_SendServerMsg("Password is Invalid", thisclient);
                        mServer.DB.FreeQuery();
                        thisclient.Client.sock.Close();
                    }
                    else
                    {
                        mServer.DB.FreeQuery();
                        mServer.DB.QStore(String.Format("CALL sp_GetAccountID('{0}');", thisclient.Session.username));
                        mServer.DB.result.Read();
                        thisclient.Session.AccountID = mServer.DB.result.GetInt32(0);
                        mServer.DB.FreeQuery();
                        thisclient.Session.ClientID = mServer.GetClientID();
                        thisclient.Session.isLoggedIn = true;
                        pak_SendCharList(pak, thisclient);
                    }
                }
            }
        }

        public void pak_SendServerMsg(string msg, CPlayer thisclient)
        {
            OutPak.Clear();
            OutPak.SetShort(108, 0); // Packet Size
            OutPak.SetShort(0x0101, 4); // Opcode
            OutPak.SetString(msg.ToString(), 12); // Msg
            thisclient.Client.encdec.Encrypt(OutPak, OutPak.dataBuffer, 108, thisclient.Client.Hash);
            thisclient.Client.SendPacket(OutPak, 108);
        }

        public void pak_SendCharList(byte[] pak, CPlayer thisclient)
        {
            OutPak.Clear();
            OutPak.SetShort(1824, 0);
            OutPak.SetShort(0x010e, 4);
            OutPak.SetShort(thisclient.Session.ClientID, 6);
            OutPak.SetString(thisclient.Session.username, 1784);

            mServer.DB.QStore(String.Format("CALL sp_GetCharInfoforCList('{0}');", thisclient.Session.AccountID));
            while (mServer.DB.result.Read())
            {
                if (mServer.DB.result.HasRows == true)
                {
                    int charslot = mServer.DB.result.GetInt32(10);
                    int charid = mServer.DB.result.GetInt32(0);
                    OutPak.SetShort(0x0841, (2 * charslot) + 12);
                    OutPak.SetShort(0x0830, (2 * charslot) + 20);
                    OutPak.SetShort(mServer.DB.result.GetInt32(3), (28 * charslot) + 92); // Level
                    OutPak.SetString(mServer.DB.result.GetString(1), (28 * charslot) + 28); // Name
                    OutPak.SetShort(mServer.DB.result.GetInt32(2), (28 * charslot) + 204); // BodyID
                    OutPak.SetShort(mServer.DB.result.GetInt32(4), (28 * charslot) + 108); // Str
                    OutPak.SetShort(mServer.DB.result.GetInt32(5), (28 * charslot) + 110); // Int
                    OutPak.SetShort(mServer.DB.result.GetInt32(6), (28 * charslot) + 112); // Dex
                    OutPak.SetShort(mServer.DB.result.GetInt32(7), (28 * charslot) + 114); // Con
                    OutPak.SetLong(mServer.DB.result.GetInt32(8), (28 * charslot) + 724); // Gold
                    OutPak.SetLong(mServer.DB.result.GetInt32(9), (28 * charslot) + 740); // Earned EXP
                    mServer.DB.QStoreExt(String.Format("CALL sp_GetInventaryItems('{0}');", charid));
                    while (mServer.DB.extresult.Read())
                    {
                        if (mServer.DB.result.HasRows == true)
                        {
                            int itemslot = mServer.DB.extresult.GetInt32(0);
                            OutPak.SetShort(mServer.DB.extresult.GetInt32(1), (8 * itemslot) + (128 * charslot) + 212); //ItemID
                            OutPak.SetByte(mServer.DB.extresult.GetInt32(2), (8 * itemslot) + (128 * charslot) + 214); // EF1
                            OutPak.SetByte(mServer.DB.extresult.GetInt32(3), (8 * itemslot) + (128 * charslot) + 215); // EFV1
                            OutPak.SetByte(mServer.DB.extresult.GetInt32(4), (8 * itemslot) + (128 * charslot) + 216); // EF2
                            OutPak.SetByte(mServer.DB.extresult.GetInt32(5), (8 * itemslot) + (128 * charslot) + 217); // EFV2
                            OutPak.SetByte(mServer.DB.extresult.GetInt32(6), (8 * itemslot) + (128 * charslot) + 218); // EF3
                            OutPak.SetByte(mServer.DB.extresult.GetInt32(7), (8 * itemslot) + (128 * charslot) + 219); // EFV3
                            mServer.DB.extresult.NextResult();
                        }
                    }
                    mServer.DB.result.NextResult();
                }
            }

            string clkeys = Encoding.UTF8.GetString(thisclient.Client.lkeys);
            OutPak.SetString(clkeys, 1800);
            thisclient.Client.encdec.Encrypt(OutPak, OutPak.dataBuffer, 1824, thisclient.Client.Hash);
            thisclient.Client.SendPacket(OutPak, 1824);
        }
    }
}

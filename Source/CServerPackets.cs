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
                case 0x020F: /* Create Char */ break;
                case 0x0211: /* Delete Char */ break;
                case 0x0366: /* Char Move */ break;
                case 0x0333: /* Chat Normal */ break;
                case 0x0334: /* Chat Whisper/Cmds */ break;
                case 0x0290: /* Request Teleport */ break;
                case 0x0376: /* Inevtory Change */ break;
                case 0x027B: /* Request NPC Shops */ break;
                case 0x039B: /* Char Attack */ break;
                case 0x0215: /* Back to Char List */ break;
                case 0x0379: /* Buy Item from Shop */ break;
                case 0x03A0: /* Unknow */ break;
                case 0x03AE: /* Client Logout from Game */ break;
                case 0x0213: pak_SendCharToGame(pak, thisclient); break;
                default: Core.CLog(String.Format("Unknow Opcode Received [0x{0:X4}]", opcode)); break;
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
                    pak_SendServerMsg("The account does not exist.", thisclient);
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
                        pak_SendServerMsg("Wrong password.", thisclient);
                        mServer.DB.FreeQuery();
                        thisclient.Client.sock.Close();
                    }
                    else
                    {
                        mServer.DB.FreeQuery();
                        mServer.DB.QStore(String.Format("CALL sp_GetAccountInfo('{0}');", thisclient.Session.username));
                        mServer.DB.result.Read();
                        thisclient.Session.AccountID = mServer.DB.result.GetInt32(0);
                        thisclient.CharInfo.sGold = mServer.DB.result.GetInt32(5);
                        mServer.DB.FreeQuery();
                        thisclient.Session.ClientID = mServer.GetClientID();
                        Core.CLog(String.Format("Assigned ClientID: {0}", thisclient.Session.ClientID));
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

        public void pak_SendCharToGame(byte[] pak, CPlayer thisclient)
        {
            int CharPos = Convert.ToInt32(pak[12]);
            thisclient.LoadCharData(CharPos);
            OutPak.Clear();
            OutPak.SetShort(1244, 0);
            OutPak.SetShort(0x0114, 4);
            OutPak.SetShort(thisclient.Session.ClientID, 6);
            OutPak.SetShort(thisclient.Position.pCurrent.x, 12);
            OutPak.SetShort(thisclient.Position.pCurrent.y, 14);
            OutPak.SetString(thisclient.CharInfo.CharName, 16);
            OutPak.SetByte(thisclient.CharInfo.Class, 36);
            OutPak.SetLong(thisclient.CharInfo.cGold, 40);
            OutPak.SetLong(thisclient.CharInfo.cExp, 44);
            OutPak.SetShort(thisclient.Position.pCurrent.x, 48);
            OutPak.SetShort(thisclient.Position.pCurrent.y, 50);
            OutPak.SetShort(thisclient.CharInfo.cLevel, 80);
            OutPak.SetShort(thisclient.Stats.mHP, 88);
            OutPak.SetShort(thisclient.Stats.mMP, 90);
            OutPak.SetShort(thisclient.Stats.cHP, 92);
            OutPak.SetShort(thisclient.Stats.cMP, 94);
            OutPak.SetShort(thisclient.Stats.STR, 96);
            OutPak.SetShort(thisclient.Stats.INT, 98);
            OutPak.SetShort(thisclient.Stats.DEX, 100);
            OutPak.SetShort(thisclient.Stats.CON, 102);
            OutPak.SetShort(thisclient.CharInfo.BodyID, 108);
            for (int i = 0; i < 78; i++)
            {
                OutPak.SetShort(thisclient.Inventory[i].ItemID, (8 * i) + 116);
                OutPak.SetByte(thisclient.Inventory[i].EF1, (8 * i) + 118);
                OutPak.SetByte(thisclient.Inventory[i].EFV1, (8 * i) + 119);
                OutPak.SetByte(thisclient.Inventory[i].EF2, (8 * i) + 120);
                OutPak.SetByte(thisclient.Inventory[i].EFV2, (8 * i) + 121);
                OutPak.SetByte(thisclient.Inventory[i].EF3, (8 * i) + 122);
                OutPak.SetByte(thisclient.Inventory[i].EFV3, (8 * i) + 123);
            }
            OutPak.SetShort(thisclient.CharInfo.StatPts, 752);
            OutPak.SetShort(thisclient.CharInfo.SkillPts, 754);
            OutPak.SetShort(thisclient.CharInfo.MasterPts, 756);
            OutPak.SetShort(thisclient.Session.ClientID, 774);
            thisclient.Client.encdec.Encrypt(OutPak, OutPak.dataBuffer, 1244, thisclient.Client.Hash);
            thisclient.Client.SendPacket(OutPak, 1244);
            thisclient.Session.isInGame = true;
            pak_SendCharToGameSpawn(pak, thisclient);
        }

        public void pak_SendCharToGameSpawn(byte[] pak, CPlayer thisclient)
        {
            OutPak.Clear();
            OutPak.SetShort(172, 0);
            OutPak.SetShort(0x0364, 4);
            OutPak.SetShort(30000, 6);
            OutPak.SetShort(thisclient.Position.pCurrent.x, 12);
            OutPak.SetShort(thisclient.Position.pCurrent.y, 14);
            OutPak.SetShort(thisclient.Session.ClientID, 16);
            OutPak.SetString(thisclient.CharInfo.CharName, 18);
            OutPak.SetShort(thisclient.CharInfo.ChaosPts, 30);
            OutPak.SetShort(thisclient.CharInfo.BodyID, 34);
            for (int i = 0; i < 15; i++)
            {
                OutPak.SetShort(thisclient.GetItemIDwRefine(thisclient.Inventory[i]), (2 * i) + 36);
            }
            OutPak.SetShort(thisclient.Session.ClientID, 66);
            OutPak.SetShort(thisclient.CharInfo.cLevel, 100);
            OutPak.SetShort(2, 107); // Move Speedy?
            OutPak.SetShort(thisclient.Stats.mHP, 108);
            OutPak.SetShort(thisclient.Stats.mMP, 110);
            OutPak.SetShort(thisclient.Stats.cHP, 112);
            OutPak.SetShort(thisclient.Stats.cMP, 114);
            OutPak.SetShort(thisclient.Stats.STR, 116);
            OutPak.SetShort(thisclient.Stats.INT, 118);
            OutPak.SetShort(thisclient.Stats.DEX, 120);
            OutPak.SetShort(thisclient.Stats.CON, 122);
            OutPak.SetShort(2, 128); // Spawn Effect 2=tele effect
            thisclient.Client.encdec.Encrypt(OutPak, OutPak.dataBuffer, 172, thisclient.Client.Hash);
            thisclient.Client.SendPacket(OutPak, 172);
        }
    }
}

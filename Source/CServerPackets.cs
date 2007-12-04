using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

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
                case 0x0366: pak_PlayerMove(pak, thisclient); break;
                case 0x0333: pak_SendChat(pak, thisclient); break;
                case 0x0334: /* Chat Whisper/Cmds */ break;
                case 0x0290: pak_Teleport(pak, thisclient); break;
                case 0x0376: /* Invetory Change */ break;
                case 0x027B: /* Request NPC Shops */ break;
                case 0x039B: /* Char Attack */ break;
                case 0x0215: /* Back to Char List */ break;
                case 0x0379: /* Buy Item from Shop */ break;
                case 0x03A0: /* Unknow */ break;
                case 0x03AE: pak_PlayerDisconnect(pak,thisclient); break;
                case 0x0213: pak_SendCharToGame(pak, thisclient); break;
                default: Core.CLog(String.Format("Unknow Opcode Received [0x{0:X4}]", opcode)); break;
            }
        }

        public void SendToVisionPlayers(CPlayer thisclient, CPacketBuilder thispacket, int size)
        {
            int lastid = 0;
            foreach(object player in thisclient.vPlayers)
            {
                if (!player.Equals(thisclient))
                {
                    CPlayer otherclient = (CPlayer)player;
                    if (otherclient.Session.ClientID != lastid)
                    {
                        otherclient.Client.SendPacket(thispacket, size);
                    }
                    lastid = otherclient.Session.ClientID;
                }
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
                    }
                    else
                    {
                        mServer.DB.FreeQuery();
                        mServer.DB.QStore(String.Format("CALL sp_GetAccountInfo('{0}');", thisclient.Session.username));
                        mServer.DB.result.Read();
                        thisclient.Session.AccountID = mServer.DB.result.GetInt32(0);
                        thisclient.CharInfo.sGold = mServer.DB.result.GetInt32(5);
                        mServer.DB.FreeQuery();
                        mServer.DB.QStore(String.Format("CALL sp_CheckAccStatus('{0}');", thisclient.Session.AccountID));
                        mServer.DB.result.Read();
                        if (mServer.DB.result.GetInt32(0) == 0)
                        {
                            mServer.DB.FreeQuery();
                            mServer.DB.QStore(String.Format("CALL sp_InsertAccStatus('{0}','{1}')", thisclient.Session.AccountID, thisclient.Client.sock.RemoteEndPoint.ToString()));
                            thisclient.Session.ClientID = mServer.GetClientID();
                            thisclient.Session.isLoggedIn = true;
                            pak_SendCharList(pak, thisclient);
                        }
                        else
                        {
                            pak_SendServerMsg("Account is already online.", thisclient);
                            mServer.DB.FreeQuery();
                        }
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
            int charslot = 0;
            int charid = 0;
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
                    charslot = mServer.DB.result.GetInt32(10);
                    charid = mServer.DB.result.GetInt32(0);
                    OutPak.SetShort(0x0841, (2 * charslot) + 12);
                    OutPak.SetShort(0x0830, (2 * charslot) + 20);
                    OutPak.SetString(mServer.DB.result.GetString(1), (16 * charslot) + 28); // Name
                    OutPak.SetShort(mServer.DB.result.GetInt32(3), (28 * charslot) + 92); // Level
                    OutPak.SetShort(mServer.DB.result.GetInt32(4), (28 * charslot) + 108); // Str
                    OutPak.SetShort(mServer.DB.result.GetInt32(5), (28 * charslot) + 110); // Int
                    OutPak.SetShort(mServer.DB.result.GetInt32(6), (28 * charslot) + 112); // Dex
                    OutPak.SetShort(mServer.DB.result.GetInt32(7), (28 * charslot) + 114); // Con
                    OutPak.SetShort(mServer.DB.result.GetInt32(2), (128 * charslot) + 204); // BodyID
                    OutPak.SetLong(mServer.DB.result.GetInt32(8), (4 * charslot) + 724); // Gold
                    OutPak.SetLong(mServer.DB.result.GetInt32(9), (4 * charslot) + 740); // Earned EXP
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
            Core.CLog(String.Format("CID: [{0}] UID: [{1}] Char: [{2}]", thisclient.Session.ClientID, thisclient.Session.username, thisclient.CharInfo.CharName));
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
            OutPak.SetShort(4, 107); // Move Speed
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
            SendToVisionPlayers(thisclient, OutPak, 172);
        }

        public void pak_SpawnChar(CPlayer thisclient, CPlayer otherclient)
        {
            OutPak.Clear();
            OutPak.SetShort(172, 0);
            OutPak.SetShort(0x0364, 4);
            OutPak.SetShort(30000, 6);
            OutPak.SetShort(otherclient.Position.pCurrent.x, 12);
            OutPak.SetShort(otherclient.Position.pCurrent.y, 14);
            OutPak.SetShort(otherclient.Session.ClientID, 16);
            OutPak.SetString(otherclient.CharInfo.CharName, 18);
            OutPak.SetShort(otherclient.CharInfo.ChaosPts, 30);
            OutPak.SetShort(otherclient.CharInfo.BodyID, 34);
            for (int i = 0; i < 15; i++)
            {
                OutPak.SetShort(thisclient.GetItemIDwRefine(otherclient.Inventory[i]), (2 * i) + 36);
            }
            OutPak.SetShort(otherclient.Session.ClientID, 66);
            OutPak.SetShort(otherclient.CharInfo.cLevel, 100);
            OutPak.SetShort(4, 107); // Move Speed
            OutPak.SetShort(otherclient.Stats.mHP, 108);
            OutPak.SetShort(otherclient.Stats.mMP, 110);
            OutPak.SetShort(otherclient.Stats.cHP, 112);
            OutPak.SetShort(otherclient.Stats.cMP, 114);
            OutPak.SetShort(otherclient.Stats.STR, 116);
            OutPak.SetShort(otherclient.Stats.INT, 118);
            OutPak.SetShort(otherclient.Stats.DEX, 120);
            OutPak.SetShort(otherclient.Stats.CON, 122);
            OutPak.SetShort(0, 128); // Spawn Effect 2=tele effect
            thisclient.Client.encdec.Encrypt(OutPak, OutPak.dataBuffer, 172, thisclient.Client.Hash);
            thisclient.Client.SendPacket(OutPak, 172);
        }

        public void pak_SpawnNpc(CPlayer thisclient, CNpc thisnpc)
        {
            thisnpc.NpcInfo.CharID = mServer.GetMobID();
            OutPak.Clear();
            OutPak.SetShort(172, 0);
            OutPak.SetShort(0x0364, 4);
            OutPak.SetShort(30000, 6);
            OutPak.SetShort(thisnpc.Position.pCurrent.x, 12);
            OutPak.SetShort(thisnpc.Position.pCurrent.y, 14);
            OutPak.SetShort(thisnpc.NpcInfo.CharID, 16);
            XmlDocument doc = new XmlDocument();
            try
            {
                doc.Load(String.Format("./Data/Npc/{0}", thisnpc.NpcFile));
            }
            catch (Exception ex)
            {
                Core.CLog(String.Format("Error: {0}", ex.Message));
            }
            XmlNode root = doc.DocumentElement;
            XmlNodeList list = root.SelectNodes("/npc/inventory/item");
            OutPak.SetString(root.Attributes.Item(0).Value, 18);
            if (int.Parse(root.Attributes.Item(2).Value).Equals(0))
                OutPak.SetShort(120, 30);
            OutPak.SetShort(int.Parse(root.Attributes.Item(1).Value), 34);
            for (int i = 0; i < list.Count; i++)
            {
                int slotnum = int.Parse(list.Item(i).Attributes.Item(0).Value);
                CItem thisitem = new CItem();
                thisitem.ItemID = int.Parse(list.Item(i).Attributes.Item(1).Value);
                thisitem.EF1 = int.Parse(list.Item(i).Attributes.Item(2).Value);
                thisitem.EFV1 = int.Parse(list.Item(i).Attributes.Item(3).Value);
                thisitem.EF2 = int.Parse(list.Item(i).Attributes.Item(4).Value);
                thisitem.EFV2 = int.Parse(list.Item(i).Attributes.Item(5).Value);
                thisitem.EF3 = int.Parse(list.Item(i).Attributes.Item(6).Value);
                thisitem.EFV3 = int.Parse(list.Item(i).Attributes.Item(7).Value);
                OutPak.SetShort(thisclient.GetItemIDwRefine(thisitem), (2 * slotnum) + 36);
            }
            OutPak.SetShort(thisnpc.NpcInfo.CharID, 66);
            OutPak.SetShort(int.Parse(root.Attributes.Item(3).Value), 100);
            OutPak.SetByte(1, 102);
            OutPak.SetByte(int.Parse(root.Attributes.Item(2).Value), 106);
            OutPak.SetShort(4, 107); // Move Speed
            OutPak.SetShort(100, 108);
            OutPak.SetShort(100, 110);
            OutPak.SetShort(100, 112);
            OutPak.SetShort(100, 114);
            OutPak.SetShort(int.Parse(root.Attributes.Item(4).Value), 116);
            OutPak.SetShort(int.Parse(root.Attributes.Item(5).Value), 118);
            OutPak.SetShort(int.Parse(root.Attributes.Item(6).Value), 120);
            OutPak.SetShort(int.Parse(root.Attributes.Item(7).Value), 122);
            OutPak.SetShort(0, 128); // Spawn Effect 2=tele effect
            thisclient.Client.encdec.Encrypt(OutPak, OutPak.dataBuffer, 172, thisclient.Client.Hash);
            thisclient.Client.SendPacket(OutPak, 172);
        }

        public void pak_DeleteCharSpawn(CPlayer thisclient, CPlayer otherclient)
        {
            OutPak.Clear();
            OutPak.SetShort(16, 0);
            OutPak.SetShort(0x0165, 4);
            OutPak.SetShort(otherclient.Session.ClientID, 6);
            thisclient.Client.encdec.Encrypt(OutPak, OutPak.dataBuffer, 16, thisclient.Client.Hash);
            thisclient.Client.SendPacket(OutPak, 16);
        }

        public void pak_DeleteNpcSpawn(CPlayer thisclient, CNpc thisnpc)
        {
            OutPak.Clear();
            OutPak.SetShort(16, 0);
            OutPak.SetShort(0x0165, 4);
            OutPak.SetShort(thisnpc.NpcInfo.CharID, 6);
            thisclient.Client.encdec.Encrypt(OutPak, OutPak.dataBuffer, 16, thisclient.Client.Hash);
            thisclient.Client.SendPacket(OutPak, 16);
        }

        public void pak_PlayerDisconnect(byte[] pak, CPlayer thisclient)
        {
            mServer.DB.QStore(String.Format("CALL sp_DeleteAccStatus('{0}')", thisclient.Session.AccountID));
            OutPak.Clear();
            OutPak.SetShort(16, 0);
            OutPak.SetShort(0x0165, 4);
            OutPak.SetShort(thisclient.Session.ClientID, 6);
            OutPak.SetShort(2, 12);
            thisclient.Client.encdec.Encrypt(OutPak, OutPak.dataBuffer, 16, thisclient.Client.Hash);
            SendToVisionPlayers(thisclient, OutPak, 16);
        }

        public void pak_Teleport(byte[] pak, CPlayer thisclient)
        {
            int price = 0;
            Random random = new Random();
            fPoint dest = new fPoint();
            dest.x = 0;
            dest.y = 0;
            XmlDocument doc = new XmlDocument();
            try
            {
                doc.Load("./Data/Teleports.xml");
            }
            catch (Exception ex)
            {
                Core.CLog(String.Format("Error: {0}", ex.Message));
            }
            XmlNode root = doc.DocumentElement;
            XmlNodeList list = root.SelectNodes("/teleports/teleport");
            for (int i = 0; i < list.Count; i++)
            {
                int sx1 = int.Parse(list.Item(i).Attributes.Item(0).Value);
                int sx2 = int.Parse(list.Item(i).Attributes.Item(1).Value);
                int sy1 = int.Parse(list.Item(i).Attributes.Item(2).Value);
                int sy2 = int.Parse(list.Item(i).Attributes.Item(3).Value);

                if (thisclient.Position.pCurrent.x >= sx1 &&
                    thisclient.Position.pCurrent.x <= sx2 &&
                    thisclient.Position.pCurrent.y <= sy1 &&
                    thisclient.Position.pCurrent.y >= sy2)
                {
                    int dx1 = int.Parse(list.Item(i).Attributes.Item(4).Value);
                    int dx2 = int.Parse(list.Item(i).Attributes.Item(5).Value);
                    int dy1 = int.Parse(list.Item(i).Attributes.Item(6).Value);
                    int dy2 = int.Parse(list.Item(i).Attributes.Item(7).Value);
                    price = int.Parse(list.Item(i).Attributes.Item(8).Value);
                    dest.x = random.Next(dx1, dx2);
                    dest.y = random.Next(dy1, dy2);
                }
            }

            if (dest.x > 0 && dest.y > 0)
            {
                if (thisclient.CharInfo.cGold < price)
                {
                    pak_SendServerMsg(String.Format("You dont have required {0} Gold to use this.", price), thisclient);
                }
                else
                {
                    thisclient.CharInfo.cGold = thisclient.CharInfo.cGold - price;
                    OutPak.Clear();
                    OutPak.SetShort(16, 0);
                    OutPak.SetShort(0x03af, 4);
                    OutPak.SetShort(thisclient.Session.ClientID, 6);
                    OutPak.SetLong(thisclient.CharInfo.cGold, 12);
                    thisclient.Client.encdec.Encrypt(OutPak, OutPak.dataBuffer, 16, thisclient.Client.Hash);
                    thisclient.Client.SendPacket(OutPak, 16);
                    pak_TeleportTo(thisclient, dest);
                }
            }
            else
            {
                pak_SendServerMsg("This Teleport is not in Database Report to Server Administrator.", thisclient);
            }
        }

        public void pak_TeleportTo(CPlayer thisclient, fPoint dest)
        {
            OutPak.Clear();
            OutPak.SetShort(52, 0);
            OutPak.SetShort(0x0366, 4);
            OutPak.SetShort(thisclient.Session.ClientID, 6);
            OutPak.SetShort(thisclient.Position.pCurrent.x, 12);
            OutPak.SetShort(thisclient.Position.pCurrent.y, 14);
            OutPak.SetShort(2, 16);
            OutPak.SetShort(1, 20);
            OutPak.SetShort(dest.x, 24);
            OutPak.SetShort(dest.y, 26);
            thisclient.Client.encdec.Encrypt(OutPak, OutPak.dataBuffer, 52, thisclient.Client.Hash);
            thisclient.Client.SendPacket(OutPak, 52);
            SendToVisionPlayers(thisclient, OutPak, 52);
        }

        public void pak_SendChat(byte[] pak, CPlayer thisclient)
        {
            OutPak.Clear();
            for (int i = 0; i < 108; i++)
                OutPak.SetByte(pak[i], i);
            thisclient.Client.encdec.Encrypt(OutPak, OutPak.dataBuffer, 108, thisclient.Client.Hash);
            SendToVisionPlayers(thisclient, OutPak, 108);
        }

        public void pak_PlayerMove(byte[] pak, CPlayer thisclient)
        {
            byte[] dx = new byte[2];
            byte[] dy = new byte[2];
            Buffer.BlockCopy(pak, 24, dx, 0, 2);
            Buffer.BlockCopy(pak, 26, dy, 0, 2);
            thisclient.Position.pCurrent.x = BitConverter.ToInt16(dx, 0);
            thisclient.Position.pCurrent.y = BitConverter.ToInt16(dy, 0);
            thisclient.Position.pDestiny.x = BitConverter.ToInt16(dx, 0);
            thisclient.Position.pDestiny.y = BitConverter.ToInt16(dy, 0);
            
            OutPak.Clear();
            for (int i = 0; i < 52; i++)
                OutPak.SetByte(pak[i], i);
            thisclient.Client.encdec.Encrypt(OutPak, OutPak.dataBuffer, 52, thisclient.Client.Hash);
            SendToVisionPlayers(thisclient, OutPak, 52);
        }
    }
}

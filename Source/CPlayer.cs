using System;
using System.Collections.Generic;
using System.Text;

namespace server
{
    public class CPlayer : CCharacter
    {
        public CClient Client;
        public CSession Session;
        public CInfo CharInfo;
        public CItem[] Inventory;

        public CPlayer(CClient sClient)
        {
            Client = sClient;
            Session = new CSession();
            Session.ClientID = 0;
            Session.CtlCode = 0;
            Session.isInGame = false;
            Session.isLoggedIn = false;
            Session.password = "";
            Session.username = "";
            CharInfo = new CInfo();
            CharInfo.CharID = 0;
            CharInfo.BodyID = 0;
            CharInfo.CharName = "";
            CharInfo.Class = 0;
            CharInfo.cLevel = 0;
            CharInfo.cExp = 0;
            CharInfo.cGold = 0;
            CharInfo.MasterPts = 0;
            CharInfo.sGold = 0;
            CharInfo.SkillPts = 0;
            CharInfo.StatPts = 0;
            Inventory = new CItem[78];
            for (int i = 0; i < 78; i++)
                ClearItem(Inventory[i]);
        }

        public void LoadCharData(int posid)
        {
            Client.Server.DB.QStore(String.Format("CALL sp_GetCharInfo({0},{1});", Session.AccountID, posid));
            while (Client.Server.DB.result.Read())
            {
                if (Client.Server.DB.result.HasRows == true)
                {
                    CharInfo.CharID = Client.Server.DB.result.GetInt32(0);
                    CharInfo.CharName = Client.Server.DB.result.GetString(1);
                    CharInfo.BodyID = Client.Server.DB.result.GetInt32(2);
                    CharInfo.cLevel = Client.Server.DB.result.GetInt32(3);
                    Stats.STR = Client.Server.DB.result.GetInt32(4);
                    Stats.INT = Client.Server.DB.result.GetInt32(5);
                    Stats.DEX = Client.Server.DB.result.GetInt32(6);
                    Stats.CON = Client.Server.DB.result.GetInt32(7);
                    CharInfo.cGold = Client.Server.DB.result.GetInt32(8);
                    CharInfo.cExp = Client.Server.DB.result.GetInt32(9);
                    CharInfo.Class = Client.Server.DB.result.GetInt32(10);
                    Stats.wMaster = Client.Server.DB.result.GetInt32(11);
                    Stats.fMaster = Client.Server.DB.result.GetInt32(12);
                    Stats.sMaster = Client.Server.DB.result.GetInt32(13);
                    Stats.tMaster = Client.Server.DB.result.GetInt32(14);
                    CharInfo.SkillPts = Client.Server.DB.result.GetInt32(15);
                    CharInfo.StatPts = Client.Server.DB.result.GetInt32(16);
                    CharInfo.MasterPts = Client.Server.DB.result.GetInt32(17);
                    CharInfo.ChaosPts = Client.Server.DB.result.GetInt32(18);
                    Stats.mHP = GetMaxHP();
                    Stats.mMP = GetMaxMP();
                    Stats.cHP = Stats.mHP;
                    Stats.cMP = Stats.mMP;
                    Random rand = new Random();
                    Position.pCurrent.x = (int)rand.Next(2090, 2096);
                    Position.pCurrent.y = (int)rand.Next(2094, 2100);
                }
            }
            Client.Server.DB.FreeQuery();

            Client.Server.DB.QStore(String.Format("CALL sp_GetInventaryItems({0});", CharInfo.CharID));
            while (Client.Server.DB.result.Read())
            {
                if (Client.Server.DB.result.HasRows == true)
                {
                    int slot = Client.Server.DB.result.GetInt32(0);
                    Inventory[slot].ItemID = Client.Server.DB.result.GetInt32(1);
                    Inventory[slot].EF1 = Client.Server.DB.result.GetInt32(2);
                    Inventory[slot].EFV1 = Client.Server.DB.result.GetInt32(3);
                    Inventory[slot].EF2 = Client.Server.DB.result.GetInt32(4);
                    Inventory[slot].EFV2 = Client.Server.DB.result.GetInt32(5);
                    Inventory[slot].EF3 = Client.Server.DB.result.GetInt32(6);
                    Inventory[slot].EFV3 = Client.Server.DB.result.GetInt32(7);
                }
            }
        }

        public int GetMaxHP()
        {
            return (Stats.CON * Client.Server.config.ConHPMulti) + (CharInfo.cLevel * Client.Server.config.LvlHPMulti) + 71;
        }

        public int GetMaxMP()
        {
            return (Stats.INT * Client.Server.config.IntMPMulti) + (CharInfo.cLevel * Client.Server.config.LvlMPMulti) + 37;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Text;

namespace server
{
    public struct fPoint
    {
        public int x;
        public int y;
    };
    public struct CSession
    {
        public int ClientID;
        public int AccountID;
        public string username;
        public string password;
        public int CtlCode;
        public bool isLoggedIn;
        public bool isInGame;
    };
    public struct CStats
    {
        public int mHP;
        public int mMP;
        public int cHP;
        public int cMP;
        public int STR;
        public int INT;
        public int DEX;
        public int CON;
        public int wMaster;
        public int fMaster;
        public int sMaster;
        public int tMaster;
    };
    public struct CPosition
    {
        public fPoint pCurrent;
        public fPoint pSource;
        public fPoint pDestiny;
    };
    public struct CItem
    {
        public int ItemID;
        public int EF1;
        public int EFV1;
        public int EF2;
        public int EFV2;
        public int EF3;
        public int EFV3;
    };
    public struct CInfo
    {
        public int CharID;
        public string CharName;
        public int ChaosPts;
        public int cLevel;
        public int BodyID;
        public int cExp;
        public int Class;
        public int cGold;
        public int sGold;
        public int StatPts;
        public int SkillPts;
        public int MasterPts;
    };
    public struct CEffect
    {
        public int Effect;
        public int Value;
    };
    public struct CItemList
    {
        public int ItemID;
        public string Name;
        public int Mesh;
        public int Type;
        public int ReqLvl;
        public int ReqStr;
        public int ReqInt;
        public int ReqDex;
        public int ReqCon;
        public int Price;
        public int Pos;
        public int Extreme;
        public int Grade;
        public CEffect[] Effects;
    };
}

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
        public ushort mHP;
        public ushort mMP;
        public ushort aHP;
        public ushort aMP;
        public ushort STR;
        public ushort INT;
        public ushort DEX;
        public ushort CON;
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
}

using System;
using System.Collections.Generic;
using System.Text;

namespace server
{
    public class CCharacter
    {
        public CStats Stats;
        public CPosition Position;

        public CCharacter()
        {
            Stats = new CStats();
            Stats.aHP = 0;
            Stats.aMP = 0;
            Stats.CON = 0;
            Stats.DEX = 0;
            Stats.INT = 0;
            Stats.mHP = 0;
            Stats.mMP = 0;
            Stats.STR = 0;
            Position = new CPosition();
            Position.pCurrent.x = 0;
            Position.pCurrent.y = 0;
            Position.pDestiny.x = 0;
            Position.pDestiny.y = 0;
            Position.pSource.x = 0;
            Position.pSource.y = 0;
        }
    }
}

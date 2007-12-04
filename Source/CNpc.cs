using System;
using System.Collections.Generic;
using System.Text;

namespace server
{
    public class CNpc : CCharacter
    {
        public CInfo NpcInfo;
        public string NpcFile;

        public CNpc()
        {
            NpcInfo = new CInfo();
            NpcInfo.BodyID = 0;
            NpcInfo.cExp = 0;
            NpcInfo.cGold = 0;
            NpcInfo.ChaosPts = 0;
            NpcInfo.CharID = 0;
            NpcInfo.CharName = null;
            NpcInfo.Class = 0;
            NpcInfo.cLevel = 0;
            NpcInfo.MasterPts = 0;
            NpcInfo.sGold = 0;
            NpcInfo.SkillPts = 0;
            NpcInfo.StatPts = 0;
            NpcFile = null;
        }
    }
}

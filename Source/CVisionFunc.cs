using System;
using System.Collections.Generic;
using System.Text;

namespace server
{
    public class CVisionFunc
    {
        public static bool isVisible(CPlayer thisclient, CPlayer otherclient)
        {
            foreach(object player in thisclient.vPlayers)
            {
                if (player.Equals(otherclient))
                    return true;
            }
            return false;
        }

        public static bool isNpcVisible(CPlayer thisclient, CNpc thisnpc)
        {
            foreach (object npc in thisclient.vNpcs)
            {
                if (npc.Equals(thisnpc))
                    return true;
            }
            return false;
        }
    }
}

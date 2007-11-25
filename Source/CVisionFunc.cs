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
    }
}

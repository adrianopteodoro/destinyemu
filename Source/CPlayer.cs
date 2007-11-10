using System;
using System.Collections.Generic;
using System.Text;

namespace server
{
    public class CPlayer : CCharacter
    {
        public CClient Client;
        public CSession Session;

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
        }
    }
}

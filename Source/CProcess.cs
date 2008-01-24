using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace server
{
    public class CProcess
    {
        public CServer Server;
        public CProcess(CServer sServer)
        {
            Server = sServer;
        }

        public void MapProcess()
        {
            while (true)
            {
                foreach (object client in Server.m_ClientList)
                {
                    CClient thisclient = (CClient)client;
                    //thisclient.Player.PlayerMovement();
                }
                Thread.Sleep(10);
            }
        }

        public void WorldProcess()
        {
            while (true)
            {
                Thread.Sleep(700);
            }
        }

        public void VisionProcess()
        {
            while (true)
            {
                foreach(object client in Server.m_ClientList)
                {
                    CClient thisclient = (CClient)client;
                    Server.VisionList(thisclient.Player);
                }
                Thread.Sleep(200);
            }
        }
    }
}

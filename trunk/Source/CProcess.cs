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
                CClient lastclient = null;
                foreach (object client in Server.m_ClientList)
                {
                    CClient thisclient = (CClient)client;
                    if (!client.Equals(lastclient))
                    {
                        //thisclient.Player.PlayerMovement();
                    }
                    lastclient = thisclient;
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
                CClient lastclient = null;
                foreach(object client in Server.m_ClientList)
                {
                    CClient thisclient = (CClient)client;
                    if (!client.Equals(lastclient))
                    {
                        Server.VisionList(thisclient.Player);
                    }
                    lastclient = thisclient;
                }
                Thread.Sleep(200);
            }
        }
    }
}

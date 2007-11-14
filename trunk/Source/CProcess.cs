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
            Thread.Sleep(10);
        }

        public void WorldProcess()
        {
            Thread.Sleep(700);
        }

        public void VisionProcess()
        {
            Thread.Sleep(200);
        }
    }
}

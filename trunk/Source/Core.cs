using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace server
{
    public class Core
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [MTAThread]
        static void Main(string[] args)
        {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.Title = "Destiny Server Emulator";
            Console.WriteLine("Destiny Server Emulator - [http://destinyemu.sf.net]");
            Console.ResetColor();
            CServer thisserver = new CServer();
            thisserver.Start();

            // Wait Infinitely
            Thread.CurrentThread.Join();

            // Close Server
            thisserver.MapThread.Join();
            thisserver.WorldThread.Join();
            thisserver.VisionThread.Join();
            Environment.Exit(0);
        }

        public static void CLog(string msg)
        {
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.Write("[{0:D02}:{1:D02}:{2:D02}] ",
                DateTime.Now.Hour,
                DateTime.Now.Minute,
                DateTime.Now.Second);
            Console.ResetColor();
            Console.ForegroundColor = ConsoleColor.Cyan;
            Console.Write("@ ");
            Console.ResetColor();
            Console.ForegroundColor = ConsoleColor.White;
            Console.WriteLine("{0}",
                msg.ToString());
            Console.ResetColor();
        }
    }
}

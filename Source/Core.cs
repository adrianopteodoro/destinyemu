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
        //[MTAThread]
        static void Main(string[] args)
        {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.Title = AssemblyInfo.Title;
            Console.WriteLine("Destiny Server Emulator - [http://destinyemu.sf.net] - v" + AssemblyInfo.AssemblyVersion);
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine(AssemblyInfo.Description);
            Console.ResetColor();
            CServer thisserver = new CServer();
            if (thisserver.DB.GetStatus())
            {
                thisserver.Start();

                // Wait Infinitely
                Thread.CurrentThread.Join();
            }
            else
                CLog("!ERROR OCCURRED! Close this window!");
			
			// Close Server
			thisserver.MapThread.Join();
			thisserver.WorldThread.Join();
			thisserver.VisionThread.Join();
			Environment.Exit(0);
        }

	/// <summary>
        /// Server Core Console Output Function
        /// </summary>
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



using System.Net;
using ServerCore;

namespace GameServer
{
    public class Program()
    {
        private static Listener _listener = new Listener();

        static void Main(string[] args)
        {
            IPAddress ipAddr = IPAddress.Parse("127.0.0.1");
            IPEndPoint endPoint = new IPEndPoint(ipAddr, 7777);
            _listener.Init(endPoint, () => { return SessionManager.Instance.Generate(); });

            Console.WriteLine("Listening...");

            // GameLogic
            const int GameThreadCount = 5;
            GameLogic.LaunchGameThreads(GameThreadCount);

            // DB

            // MainThread
            GameLogic.FlushMainThreadJobs();
        }
    }
}
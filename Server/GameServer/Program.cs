

using System.Net;
using ServerCore;

namespace GameServer
{
    public class Program()
    {
        private static Listener _listener = new Listener();

        static void Main(string[] args)
        {
            ConfigManager.LoadConfig();
            IPAddress ipAddr = IPAddress.Parse(ConfigManager.Config.ip);
            IPEndPoint endPoint = new IPEndPoint(ipAddr, ConfigManager.Config.port);
            _listener.Init(endPoint, () => { return SessionManager.Instance.Generate(); });

            Console.WriteLine("Listening...");

            // GameLogic
            const int GameThreadCount = 2;
            GameLogic.LaunchGameThreads(GameThreadCount);

            // DB

            // MainThread
            GameLogic.FlushMainThreadJobs();
        }
    }
}
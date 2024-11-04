using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameServer
{
    public class GameLogic : JobSerializer
    {
        public static GameLogic Instance { get; } = new GameLogic();

        static Dictionary<int, GameRoom> _rooms;
        static int _roomIdGenerator = 1;
        static int _threadCount;

        static MatchMaking _matchMaking;

        static public ConcurrentQueue<GameRoom> _updateQueue;

        public GameLogic()
        {
            _rooms = new Dictionary<int, GameRoom>();
            _roomIdGenerator = 1;
            _updateQueue = new ConcurrentQueue<GameRoom>();
        }

        // 매칭완료시 새로운 Room 생성
        static public GameRoom Add()
        {
            GameRoom gameRoom = new GameRoom();
            gameRoom.Init();

            gameRoom.GameRoomId = _roomIdGenerator;
            _rooms.Add(_roomIdGenerator, gameRoom);
            _roomIdGenerator++;
            return gameRoom;
        }

        static public void FlushMainThreadJobs()
        {
            Thread.CurrentThread.Name = "MainThread";

            while (true)
            {
                Instance.Flush();
                Thread.Sleep(0);
            }
        }

        static public void LaunchGameThreads(int threadCount)
        {
            _threadCount = threadCount;

            for (int i = 0; i < threadCount; i++)
            {
                Thread t = new Thread(new ParameterizedThreadStart(GameThreadJob));
                t.Name = $"GameLogic_{i}";
                t.Start(i);
            }
        }

        static public void GameThreadJob(object arg)
        {
            int threadId = (int)arg;

            while (true)
            {
                if (_updateQueue.TryDequeue(out GameRoom gameRoom) == false)
                {
                    continue;
                }

                gameRoom.Flush();

                _updateQueue.Enqueue(gameRoom);

                Thread.Sleep(0);
            }
        }
    }
}

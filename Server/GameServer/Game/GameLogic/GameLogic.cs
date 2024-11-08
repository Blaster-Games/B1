using Google.Protobuf.Protocol;
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

        // 게임룸 관련
        static Dictionary<int, GameRoom> _rooms = new Dictionary<int, GameRoom>();
        static int _roomIdGenerator = 1;
        static int _threadCount;
        static public ConcurrentQueue<GameRoom> _updateQueue = new ConcurrentQueue<GameRoom>();

        // 로비 관련
        static LobbyRoom _lobbyRoom;

        public GameLogic()
        {
            // 로비 초기화
            _lobbyRoom = new LobbyRoom();
            _lobbyRoom.Init();
        }

        #region Room 관리
        static public GameRoom Add()
        {
            GameRoom gameRoom = new GameRoom();
            gameRoom.Init();
            gameRoom.GameRoomId = _roomIdGenerator;
            _rooms.Add(_roomIdGenerator, gameRoom);
            _roomIdGenerator++;

            // 실시간 업데이트가 필요한 게임룸만 큐에 추가
            _updateQueue.Enqueue(gameRoom);
            return gameRoom;
        }

        static public void Remove(int roomId)
        {
            if (_rooms.ContainsKey(roomId))
                _rooms.Remove(roomId);
        }
        #endregion

        #region 로비 관리
        public void EnterLobby(ClientSession session)
        {
            // 로비 입장은 메인스레드에서 처리
            Push(() =>
            {
                _lobbyRoom.Enter(session);
            });
        }

        public void LeaveLobby(ClientSession session)
        {
            Push(() =>
            {
                _lobbyRoom.Leave(session);
            });
        }

        public void HandleChat(ClientSession session, C_LobbyChat chatPacket)
        {
            Push(() =>
            {
                _lobbyRoom.HandleChat(session, chatPacket);
            });
        }
        #endregion

        #region Thread 관리
        static public void FlushMainThreadJobs()
        {
            Thread.CurrentThread.Name = "MainThread";
            while (true)
            {
                // 메인스레드에서 로비 관련 작업 처리
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
                // 게임 룸만 실시간 업데이트
                if (_updateQueue.TryDequeue(out GameRoom gameRoom) == false)
                {
                    continue;
                }
                gameRoom.Flush();
                _updateQueue.Enqueue(gameRoom);
                Thread.Sleep(0);
            }
        }
        #endregion
    }
}

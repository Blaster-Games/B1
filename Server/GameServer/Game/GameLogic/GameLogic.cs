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
        #region Singleton
        private static readonly Lazy<GameLogic> _instance = new Lazy<GameLogic>(() => new GameLogic());
        public static GameLogic Instance { get { return _instance.Value; } }
        #endregion

        #region Fields
        // Room 관련
        private static Dictionary<int, GameRoom> _rooms = new Dictionary<int, GameRoom>();
        private static int _roomIdGenerator = 1;
        private static int _threadCount;
        public static ConcurrentQueue<GameRoom> _updateQueue = new ConcurrentQueue<GameRoom>();

        // Lobby 관련 
        private static LobbyRoom _lobbyRoom;

        // Locks
        private static readonly object _roomLock = new object();
        #endregion

        #region Constructors
        static GameLogic()
        {
            Console.WriteLine("Static 생성자 실행");
            _roomIdGenerator = 1;
            _updateQueue = new ConcurrentQueue<GameRoom>();
            Console.WriteLine($"룸 Dictionary Count: {_rooms.Count}");
        }

        private GameLogic()
        {
            Console.WriteLine("일반 생성자 실행 시작");
            _lobbyRoom = new LobbyRoom();
            _lobbyRoom.Init();
        }
        #endregion

        #region Room Management
        public List<RoomListItemInfo> GetRoomListItems()
        {
            Console.WriteLine($"방 목록 요청 - 현재 총 방 개수: {_rooms.Count}");
            var roomList = _rooms.Values.Select(room => new RoomListItemInfo
            {
                RoomId = room.GameRoomId,
                RoomName = room.RoomName,
                RoomType = room.GameMode,
                CurrentPlayers = room.CurrentPlayerCount,
                MaxPlayers = room.MaxPlayers,
                State = room.State,
                MapName = room.MapName
            }).ToList();
            Console.WriteLine($"변환된 방 목록 개수: {roomList.Count}");

            foreach (var room in roomList)
            {
                Console.WriteLine($"방 정보 - ID: {room.RoomId}, 이름: {room.RoomName}, 상태: {room.State}");
            }

            return roomList;
        }

        public GameRoom AddRoom(GameRoom room, Action<GameRoom> callback)
        {
            room.GameRoomId = _roomIdGenerator;
            _rooms.Add(_roomIdGenerator, room);
            _roomIdGenerator++;
            _updateQueue.Enqueue(room);

            callback?.Invoke(room);
            return room;
        }

        public void RemoveRoom(int roomId)
        {
            lock (_roomLock)
            {
                if (_rooms.TryGetValue(roomId, out GameRoom room))
                {
                    _rooms.Remove(roomId);
                    room.State = ERoomState.StateTerminated;
                }
            }
        }

        public GameRoom FindRoom(int roomId)
        {
            if (_rooms.TryGetValue(roomId, out GameRoom room))
            {
                return room;
            }
            return null;
        }

        public void TryEnterRoom(ClientSession session, int roomId, Action<bool, GameRoom> callback)
        {
            Push(() =>
            {
                GameRoom room = FindRoom(roomId);
                if (room == null)
                {
                    callback.Invoke(false, null);
                    return;
                }
                room.Push(() =>
                {
                    room.EnterRoom(session, (success) => callback.Invoke(success, room));
                });
            });
        }
        #endregion

        #region Lobby Management
        public void EnterLobby(ClientSession session)
        {
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

        #region Thread Management
        public static void FlushMainThreadJobs()
        {
            Thread.CurrentThread.Name = "MainThread";
            while (true)
            {
                Instance.Flush();
                Thread.Sleep(0);
            }
        }

        public static void LaunchGameThreads(int threadCount)
        {
            _threadCount = threadCount;
            for (int i = 0; i < threadCount; i++)
            {
                Thread t = new Thread(new ParameterizedThreadStart(GameThreadJob));
                t.Name = $"GameLogic_{i}";
                t.Start(i);
            }
        }

        public static void GameThreadJob(object arg)
        {
            int threadId = (int)arg;
            while (true)
            {
                if (_updateQueue.TryDequeue(out GameRoom gameRoom) == false)
                {
                    continue;
                }
                if (gameRoom.State != ERoomState.StateTerminated)
                {
                    gameRoom.Flush();
                    _updateQueue.Enqueue(gameRoom);
                }
                Thread.Sleep(0);
            }
        }
        #endregion
    }
}
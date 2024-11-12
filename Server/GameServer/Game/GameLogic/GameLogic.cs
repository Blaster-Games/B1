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
        private static readonly Lazy<GameLogic> _instance = new Lazy<GameLogic>(() => new GameLogic());
        public static GameLogic Instance { get { return _instance.Value; } }

        // 게임룸 관련
        static readonly Dictionary<int, GameRoom> _rooms = new Dictionary<int, GameRoom>();
        static int _roomIdGenerator = 1;
        static int _threadCount;
        static public ConcurrentQueue<GameRoom> _updateQueue = new ConcurrentQueue<GameRoom>();

        // 로비 관련
        static LobbyRoom _lobbyRoom;

        static readonly object _roomLock = new object();

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

            Console.WriteLine($"더미룸 생성 전 Dictionary Count: {_rooms.Count}");
            CreateDummyRooms();
            Console.WriteLine($"더미룸 생성 후 Dictionary Count: {_rooms.Count}");
        }

        #region 룸 관리

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

            // 각 방의 정보도 출력
            foreach (var room in roomList)
            {
                Console.WriteLine($"방 정보 - ID: {room.RoomId}, 이름: {room.RoomName}, 상태: {room.State}");
            }

            return roomList;
        }

        public GameRoom AddRoom(GameRoom room)
        {
            Console.WriteLine($"룸 생성! 이름: {room.RoomName}, 현재 총 방 개수: {_rooms.Count}");
            lock (_roomLock)
            {
                room.GameRoomId = _roomIdGenerator;
                _rooms.Add(_roomIdGenerator, room);
                _roomIdGenerator++;
                _updateQueue.Enqueue(room);
                Console.WriteLine($"룸 추가 완료! ID: {room.GameRoomId}, 추가 후 총 방 개수: {_rooms.Count}");
                return room;
            }
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

        // 락 걸고 써야한다
        public GameRoom FindRoom(int roomId)
        {
            if (_rooms.TryGetValue(roomId, out GameRoom room))
            {
                return room;
            }
            return null;
        }

        public void TryEnterRoom(ClientSession session, int roomId, Action<bool> callback)
        {
            GameRoom room;
            lock (_roomLock)
            {
                room = FindRoom(roomId);
                if (room == null)
                {
                    callback.Invoke(false);
                    return;
                }
                room.EnterRoom(session, callback);
            }
        }

        #endregion

        #region 로비 관리
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

        #region Thread 관리
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
                // 게임 룸만 실시간 업데이트
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

        private void CreateDummyRooms()
        {
            for (int i = 1; i <= 3; i++)
            {
                GameRoom room = new GameRoom();
                room.Init();
                room.RoomName = $"Test Room {i}";
                room.GameMode = EGameMode.ModeTeamdeathmatch;
                room.MaxPlayers = 8;
                room.MapName = $"Map_{i}";
                room.State = ERoomState.StateWaiting;

                // 모든 속성 설정 후 AddRoom
                AddRoom(room);
            }
        }
    }
}

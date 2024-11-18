using Google.Protobuf.Protocol;
using System;
using System.Collections.Generic;
using System.Linq;

namespace GameServer
{
    public class GameRoom : JobSerializer
    {
        // Room 기본 정보
        public int GameRoomId { get; set; }
        public string RoomName { get; set; }
        public EGameMode GameMode { get; set; }
        public ERoomState State { get; set; }
        public string MapName { get; set; }
        public int MaxPlayers { get; set; }

        // Player 관리
        private List<Player> _players = new List<Player>();
        public IReadOnlyList<Player> Players => _players;

        private Player _host;
        public Player Host => _host;

        public int CurrentPlayerCount => _players.Count;
        public int HostPlayerId => _host?.PlayerId ?? 0;

        public void Init()
        {
            _players = new List<Player>();
            State = ERoomState.StateWaiting;
            MaxPlayers = 8;
            GameMode = EGameMode.ModeTeamdeathmatch;
            RoomName = "";
            MapName = "";
        }

        public void EnterRoom(ClientSession session, Action<bool> callback)
        {
            if (State == ERoomState.StateTerminated)
            {
                callback.Invoke(false);
                return;
            }
            if (_players.Count >= MaxPlayers)
            {
                callback.Invoke(false);
                return;
            }

            Player player = session.Player;
            if (player == null)
            {
                callback.Invoke(false);
                return;
            }

            // 1. 새로운 플레이어 정보 설정
            if (_players.Count == 0)
            {
                _host = player;
                player.IsHost = true;
                player.GameRoom = this;
                player.RoomId = GameRoomId;
            }
            else
            {
                player.IsHost = false;
                player.GameRoom = this;
                player.RoomId = GameRoomId;
            }

            // 2. 플레이어 리스트에 추가
            _players.Add(player);

            // 3. 기존 플레이어들에게 새 플레이어 입장을 브로드캐스트
            BroadcastEnterGame();

            // 4. 성공 콜백
            callback.Invoke(true);
        }


        public void LeaveRoom(ClientSession session, Action<bool> callback)
        {
            // TODO
        }

        public void StartGame(string hostAddress, int hostPort)
        {
            // 호스트를 제외한 모든 플레이어에게 브로드 캐스팅
            foreach (Player p in _players)
            {
                if (p.IsHost == false)
                {
                    S_BroadcastStartGame startPacket = new S_BroadcastStartGame()
                    {
                        HostAddress = hostAddress,
                        Port = hostPort
                    };

                    p.Session?.Send(startPacket);
                }
            }
        }

        public void LeaveGame(ClientSession session)
        {
            Push(() =>
            {
                Player player = _players.Find(p => p.PlayerId == session.SessionId);
                if (player == null)
                    return;

                _players.Remove(player);

                // TODO : 방장이 나가면 다음 사람에게 방장 위임

                // TODO : 퇴장 브로드캐스트

                // 모두 나가면 방 삭제 요청
                if (_players.Count == 0)
                {
                    GameLogic.Instance.RemoveRoom(GameRoomId);
                }
            });
        }

        #region 패킷 브로드캐스트

        private void BroadcastEnterGame()
        {
            S_BroadcastJoinRoom enterPacket = new S_BroadcastJoinRoom()
            {
                Room = ToRoomDetail()
            };

            // 새로운 플레이어를 제외한 기존 플레이어들에게만 브로드캐스트
            foreach (Player p in _players)
            {
                p.Session?.Send(enterPacket);
            }
        }

        public void BroadcastChat(Player sender, string message)
        {
            // 로그: 브로드캐스트 시작
            Console.WriteLine($"[BroadcastChat] Starting broadcast from Player {sender.PlayerId} ({sender.PlayerName})");
            Console.WriteLine($"[BroadcastChat] Message to broadcast: {message}");

            S_BroadcastRoomChat packet = new S_BroadcastRoomChat()
            {
                PlayerId = sender.PlayerId,
                PlayerName = sender.PlayerName,
                Message = message
            };

            // 로그: 현재 방 인원 수
            Console.WriteLine($"[BroadcastChat] Broadcasting to {_players.Count} players");

            foreach (Player p in _players)
            {
                if (p.Session != null)
                {
                    p.Session.Send(packet);
                    Console.WriteLine($"[BroadcastChat] Sent to Player {p.PlayerId} ({p.PlayerName})");
                }
                else
                {
                    Console.WriteLine($"[BroadcastChat] Skipped Player {p.PlayerId} - null session");
                }
            }

            // 로그: 브로드캐스트 완료
            Console.WriteLine($"[BroadcastChat] Broadcast complete");
        }

        #endregion

        #region Room Info
        // Room List용 정보
        public RoomListItemInfo ToRoomListItem()
        {
            return new RoomListItemInfo
            {
                RoomId = GameRoomId,
                RoomName = RoomName,
                RoomType = GameMode,
                CurrentPlayers = CurrentPlayerCount,
                MaxPlayers = MaxPlayers,
                State = State,
                MapName = MapName
            };
        }

        // Room Detail 정보
        public RoomDetailInfo ToRoomDetail()
        {
            RoomDetailInfo detail = new RoomDetailInfo
            {
                RoomId = GameRoomId,
                RoomName = RoomName,
                RoomType = GameMode,
                MaxPlayers = MaxPlayers,
                State = State,
                MapName = MapName,
                HostPlayerId = HostPlayerId
            };

            // 현재 참가중인 모든 플레이어 정보
            detail.Players.AddRange(_players.Select(p => new PlayerInfo
            {
                PlayerId = p.PlayerId,
                PlayerName = p.PlayerName,
                IsHost = p.IsHost,
                Team = p.Team
            }));

            return detail;
        }

        public List<PlayerInfo> GetPlayerInfos()
        {
            return _players.Select(p => new PlayerInfo
            {
                PlayerId = p.PlayerId,
                PlayerName = p.PlayerName,
                IsHost = p.IsHost,
                Team = p.Team
            }).ToList();
        }
        #endregion
    }
}
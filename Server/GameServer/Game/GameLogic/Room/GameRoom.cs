using Google.Protobuf.Protocol;
using System;
using System.Collections.Generic;
using System.Linq;

namespace GameServer
{
    public class GameRoom : JobSerializer
    {
        #region Properties
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
        #endregion

        #region Slot Management
        private class SlotInfo
        {
            public bool IsOccupied;
            public Player Player;
            public ETeamType Team;
        }
        private SlotInfo[] _slots = new SlotInfo[8];

        private int FindAvailableSlot()
        {
            // 각 팀 인원 계산
            int redCount = 0, blueCount = 0;
            for (int i = 0; i < _slots.Length; i++)
            {
                if (_slots[i].IsOccupied)
                {
                    if (i < 4) redCount++;
                    else blueCount++;
                }
            }

            // 인원이 적은 팀 우선 배정
            bool tryRedFirst = (redCount <= blueCount);

            // 첫 번째 시도 (선호 팀)
            for (int i = 0; i < _slots.Length; i++)
            {
                bool isRedSlot = i < 4;
                if (!_slots[i].IsOccupied && isRedSlot == tryRedFirst)
                    return i;
            }

            // 두 번째 시도 (반대 팀)
            for (int i = 0; i < _slots.Length; i++)
            {
                if (!_slots[i].IsOccupied)
                    return i;
            }

            return -1;
        }
        #endregion

        #region Room Management
        public void Init()
        {
            _players = new List<Player>();
            State = ERoomState.StateWaiting;
            MaxPlayers = 8;
            GameMode = EGameMode.ModeTeamdeathmatch;
            RoomName = "";
            MapName = "Highrise";

            _slots = new SlotInfo[8];
            for (int i = 0; i < 8; i++)
            {
                _slots[i] = new SlotInfo
                {
                    IsOccupied = false,
                    Player = null,
                    Team = i < 4 ? ETeamType.TeamRed : ETeamType.TeamBlue
                };
            }
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

            // 사용 가능한 슬롯 찾기
            int slotIndex = FindAvailableSlot();
            if (slotIndex == -1)
            {
                callback.Invoke(false);
                return;
            }

            // 1. 새로운 플레이어 정보 설정
            if (_players.Count == 0)
            {
                _host = player;
                player.IsHost = true;
            }
            else
            {
                player.IsHost = false;
            }

            // 슬롯 할당 및 팀 설정
            _slots[slotIndex].IsOccupied = true;
            _slots[slotIndex].Player = player;
            player.Team = _slots[slotIndex].Team;
            player.GameRoom = this;
            player.RoomId = GameRoomId;
            player.SlotNumber = slotIndex;

            // 2. 플레이어 리스트에 추가
            _players.Add(player);

            // 3. 기존 플레이어들에게 새 플레이어 입장을 브로드캐스트
            BroadcastEnterGame();

            // 4. 성공 콜백
            callback.Invoke(true);
        }

        public void LeaveRoom(ClientSession session, Action<bool> callback)
        {
            Console.WriteLine($"[LeaveRoom] Processing leave request for Player {session.SessionId}");

            Player player = _players.Find(p => p.PlayerId == session.SessionId);
            if (player == null)
            {
                Console.WriteLine($"[LeaveRoom] Player {session.SessionId} not found in room");
                callback?.Invoke(false);
                return;
            }

            Console.WriteLine($"[LeaveRoom] Found player - Name: {player.PlayerName}, IsHost: {player.IsHost}");

            // 슬롯에서 제거
            for (int i = 0; i < _slots.Length; i++)
            {
                if (_slots[i].Player == player)
                {
                    Console.WriteLine($"[LeaveRoom] Removing player from slot {i}");
                    _slots[i].IsOccupied = false;
                    _slots[i].Player = null;
                    break;
                }
            }

            _players.Remove(player);
            Console.WriteLine($"[LeaveRoom] Removed player from player list. Remaining players: {_players.Count}");

            // 호스트 변경 처리
            if (player.IsHost && _players.Count > 0)
            {
                Console.WriteLine($"[LeaveRoom] Host left the room. Assigning new host...");
                _host = _players[0];
                _host.IsHost = true;
                Console.WriteLine($"[LeaveRoom] New host assigned - Name: {_host.PlayerName}, ID: {_host.PlayerId}");
            }

            // 퇴장 브로드캐스트
            Console.WriteLine($"[LeaveRoom] Broadcasting leave message to remaining players");
            BroadcastLeaveGame();

            // 모두 나가면 방 삭제 요청
            if (_players.Count == 0)
            {
                Console.WriteLine($"[LeaveRoom] Room is empty. Requesting room removal - RoomId: {GameRoomId}");
                GameLogic.Instance.RemoveRoom(GameRoomId);
            }

            Console.WriteLine($"[LeaveRoom] Leave process completed for Player {player.PlayerId}");

            callback?.Invoke(true);
        }

        public void LeaveRoom(ClientSession clientSession)
        {
            Console.WriteLine($"[LeaveRoom] Processing leave request for Player {clientSession.SessionId}");
            Player player = _players.Find(p => p.PlayerId == clientSession.SessionId);
            if (player == null)
            {
                Console.WriteLine($"[LeaveRoom] Player {clientSession.SessionId} not found in room");
                return;
            }

            Console.WriteLine($"[LeaveRoom] Found player - Name: {player.PlayerName}, IsHost: {player.IsHost}");

            // 슬롯에서 제거
            for (int i = 0; i < _slots.Length; i++)
            {
                if (_slots[i].Player == player)
                {
                    Console.WriteLine($"[LeaveRoom] Removing player from slot {i}");
                    _slots[i].IsOccupied = false;
                    _slots[i].Player = null;
                    break;
                }
            }

            _players.Remove(player);
            Console.WriteLine($"[LeaveRoom] Removed player from player list. Remaining players: {_players.Count}");

            // 호스트 변경 처리
            if (player.IsHost && _players.Count > 0)
            {
                Console.WriteLine($"[LeaveRoom] Host left the room. Assigning new host...");
                _host = _players[0];
                _host.IsHost = true;
                Console.WriteLine($"[LeaveRoom] New host assigned - Name: {_host.PlayerName}, ID: {_host.PlayerId}");
            }

            // 퇴장 브로드캐스트
            Console.WriteLine($"[LeaveRoom] Broadcasting leave message to remaining players");
            BroadcastLeaveGame();

            // 모두 나가면 방 삭제 요청
            if (_players.Count == 0)
            {
                Console.WriteLine($"[LeaveRoom] Room is empty. Requesting room removal - RoomId: {GameRoomId}");
                GameLogic.Instance.RemoveRoom(GameRoomId);
            }

            Console.WriteLine($"[LeaveRoom] Leave process completed for Player {player.PlayerId}");
        }

        public void StartGame(ClientSession clientSession, int hostPort)
        {
            Console.WriteLine($"Total players: {_players.Count}");

            // 호스트를 제외한 모든 플레이어에게 브로드 캐스팅
            int broadcastCount = 0;
            foreach (Player p in _players)
            {
                if (p.IsHost == false)
                {
                    S_BroadcastStartGame startPacket = new S_BroadcastStartGame()
                    {
                        HostAddress = clientSession.IpAddress,
                        Port = hostPort
                    };
                    Console.WriteLine($"HostAddress: {startPacket.HostAddress}, Port: {startPacket.Port}");

                    try
                    {
                        p.Session?.Send(startPacket);
                        broadcastCount++;
                        Console.WriteLine($"Successfully sent packet to Player {p.PlayerId}");
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine($"Failed to send packet to Player {p.PlayerId}: {ex.Message}");
                    }
                }
                else
                {
                    Console.WriteLine($"Skipping host player {p.PlayerId}");
                }
            }

            Console.WriteLine($"StartGame completed - Broadcast to {broadcastCount} players");

            var playersToRemove = _players.ToList();

            // 각 플레이어에 대해 LeaveRoom 호출
            foreach (var player in playersToRemove)
            {
                Console.WriteLine($"Removing player {player.PlayerId} from room after game start");
                if (player.Session != null)
                {
                    LeaveRoom(player.Session);
                }
            }

            Console.WriteLine("All players removed from room after game start");
        }
        #endregion

        #region Broadcasting
        private void BroadcastEnterGame()
        {
            S_BroadcastJoinRoom enterPacket = new S_BroadcastJoinRoom()
            {
                Room = ToRoomDetail()
            };

            foreach (Player p in _players)
            {
                p.Session?.Send(enterPacket);
            }
        }

        public void BroadcastLeaveGame()
        {
            Console.WriteLine($"[BroadcastLeaveGame] Starting broadcast - Current player count: {_players.Count}");

            S_BroadcastLeaveRoom leavePacket = new S_BroadcastLeaveRoom()
            {
                Room = ToRoomDetail(),
            };

            // 리스트를 복사하여 순회
            var playersCopy = _players.ToList();

            foreach (Player p in playersCopy)
            {
                if (p == null)
                {
                    Console.WriteLine($"[BroadcastLeaveGame] Player is null - skipping");
                    continue;
                }
                Console.WriteLine($"[BroadcastLeaveGame] Sending packet to Player {p.PlayerId} ({p.PlayerName})");
                p.Session?.Send(leavePacket);
            }

            Console.WriteLine("[BroadcastLeaveGame] Broadcast completed");
        }
        public void BroadcastChat(Player sender, string message)
        {
            Console.WriteLine($"[BroadcastChat] Starting broadcast from Player {sender.PlayerId} ({sender.PlayerName})");
            Console.WriteLine($"[BroadcastChat] Message to broadcast: {message}");

            S_BroadcastRoomChat packet = new S_BroadcastRoomChat()
            {
                PlayerId = sender.PlayerId,
                PlayerName = sender.PlayerName,
                Message = message
            };

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

            Console.WriteLine($"[BroadcastChat] Broadcast complete");
        }
        #endregion

        #region Room Info
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

            detail.Players.AddRange(_players.Select(p => new PlayerInfo
            {
                PlayerId = p.PlayerId,
                PlayerName = p.PlayerName,
                IsHost = p.IsHost,
                Team = p.Team,
                SlotNumber = p.SlotNumber
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
                Team = p.Team,
                SlotNumber = p.SlotNumber
            }).ToList();
        }
        #endregion
    }
}
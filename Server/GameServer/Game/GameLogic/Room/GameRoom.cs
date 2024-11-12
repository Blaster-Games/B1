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
            Push(() =>
            {
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

                if (_players.Count == 0)
                {
                    _host = player;
                    player.IsHost = true;
                }

                _players.Add(player);
                //BroadcastEnterGame(player);

                callback.Invoke(true);
            });
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
        private void BroadcastEnterGame(Player newPlayer)
        {
            S_BroadcastJoinRoom enterPacket = new S_BroadcastJoinRoom()
            {
                NewPlayer = new PlayerInfo()
                {
                    PlayerId = newPlayer.PlayerId,
                    PlayerName = newPlayer.PlayerName,
                    IsHost = newPlayer.IsHost,
                    Team = newPlayer.Team
                }
            };

            foreach (Player p in _players)
            {
                p.Session?.Send(enterPacket);
            }
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
        #endregion
    }
}
using Google.Protobuf.Protocol;
using Microsoft.Identity.Client;
using ServerCore;
using System;
using System.Collections.Generic;
using System.IdentityModel.Tokens.Jwt;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameServer
{
    public partial class ClientSession : PacketSession
    {
        public bool Authenticated { get; private set; } = false;

        public void HandleAuthReq(C_AuthReq reqPacket)
        {
            Console.WriteLine($"Received Auth Packet - AccountId: {reqPacket.AccountDbId}, Nickname: '{reqPacket.Nickname}'");
            Console.WriteLine($"Nickname length: {reqPacket.Nickname?.Length ?? 0}");

            if (Utils.ValidateJwtAccessToken(reqPacket.Jwt, ConfigManager.Config.secretKey) == false)
                return;

            Authenticated = true;
            AccountDbId = reqPacket.AccountDbId;
            Player.PlayerName = reqPacket.Nickname;
            AccessToken = reqPacket.Jwt;

            Console.WriteLine("접속 유저 ID : " + AccountDbId);
            Console.WriteLine("접속 유저 토큰 : " + AccessToken);

            S_AuthRes resPacket = new S_AuthRes();
            resPacket.Success = true;
            resPacket.PlayerId = Player.PlayerId;

            Console.WriteLine("접속유저 플레이어 ID : " + resPacket.PlayerId);
            Console.WriteLine("접속유저 세션 ID : " + this.SessionId);

            Send(resPacket);
        }

        public void HandleEnterLobbyReq(C_EnterLobbyReq reqPacket)
        {
            GameLogic.Instance.EnterLobby(this);
            S_EnterLobbyRes resPacket = new S_EnterLobbyRes();
            Send(resPacket);
        }

        public void HandleRoomListReq(C_RoomListReq reqPacket)
        {
            Console.WriteLine($"[RoomListReq] {this.Player.PlayerName}님이 방을 새로고침 했습니다.");

            // GameLogic의 _rooms 상태 확인
            Console.WriteLine($"[RoomListReq] GameLogic 호출 전 시점");

            List<RoomListItemInfo> roomInfoList = GameLogic.Instance.GetRoomListItems();

            Console.WriteLine($"[RoomListReq] 받아온 룸 리스트 개수: {roomInfoList?.Count ?? 0}");

            if (roomInfoList != null && roomInfoList.Count > 0)
            {
                foreach (var room in roomInfoList)
                {
                    Console.WriteLine($"[RoomListReq] 방 정보 - ID: {room.RoomId}, 이름: {room.RoomName}");
                }
            }
            else
            {
                Console.WriteLine("[RoomListReq] 방 목록이 비어있습니다!");
            }

            S_RoomListRes resPacket = new S_RoomListRes();
            resPacket.Rooms.AddRange(roomInfoList);

            Console.WriteLine($"[RoomListReq] 응답 패킷에 포함된 방 개수: {resPacket.Rooms.Count}");

            Send(resPacket);
        }

        public void HandleCreateRoomReq(C_CreateRoomReq reqPacket)
        {
            Console.WriteLine($"[HandleCreateRoomReq] 방 생성 요청 받음 - 제목: {reqPacket.Title}, 모드: {reqPacket.Mode}, 최대인원: {reqPacket.MaxPlayers}");

            GameRoom room = new GameRoom();
            room.Init();
            room.RoomName = reqPacket.Title;
            room.GameMode = reqPacket.Mode;
            room.MaxPlayers = reqPacket.MaxPlayers;
            room.MapName = reqPacket.MapName;
            room.State = ERoomState.StateWaiting;

            Console.WriteLine($"[HandleCreateRoomReq] 방 객체 생성 완료 - 이름: {room.RoomName}, 상태: {room.State}");

            GameLogic.Instance.Push(() =>
            {
                Console.WriteLine("[HandleCreateRoomReq] GameLogic Push 시작");
                GameLogic.Instance.AddRoom(room, (createdRoom) =>
                {
                    Console.WriteLine($"[HandleCreateRoomReq] 방 추가 완료 - 방 ID: {createdRoom.GameRoomId}");

                    createdRoom.Push(() =>
                    {
                        Console.WriteLine($"[HandleCreateRoomReq] 방 입장 처리 시작 - 방 ID: {createdRoom.GameRoomId}");
                        createdRoom.EnterRoom(this, (success) =>
                        {
                            Console.WriteLine($"[HandleCreateRoomReq] 방 입장 결과 - 성공여부: {success}");

                            // 플레이어 정보 상세 로깅
                            var playerInfos = createdRoom.GetPlayerInfos();
                            Console.WriteLine("\n[Player Details]");
                            foreach (var player in playerInfos)
                            {
                                Console.WriteLine($"플레이어 ID: {player.PlayerId}" +
                                    $"\n\t이름: {player.PlayerName}" +
                                    $"\n\t호스트 여부: {player.IsHost}" +
                                    $"\n\t팀: {player.Team}" +
                                    $"\n\t슬롯 번호: {player.SlotNumber}" +
                                    "\n----------------------------------------");
                            }

                            S_CreateRoomRes resPacket = new S_CreateRoomRes()
                            {
                                Success = success,
                                Room = new RoomDetailInfo()
                                {
                                    RoomId = createdRoom.GameRoomId,
                                    RoomName = createdRoom.RoomName,
                                    RoomType = createdRoom.GameMode,
                                    MaxPlayers = createdRoom.MaxPlayers,
                                    State = createdRoom.State,
                                    MapName = createdRoom.MapName,
                                    HostPlayerId = createdRoom.Host?.PlayerId ?? 0,
                                    Players = { playerInfos }
                                }
                            };

                            Console.WriteLine($"[HandleCreateRoomReq] 응답 패킷 전송 - 방 ID: {resPacket.Room.RoomId}, " +
                                $"호스트 ID: {resPacket.Room.HostPlayerId}, " +
                                $"플레이어 수: {resPacket.Room.Players.Count}");

                            this.Send(resPacket);
                        });
                    });
                });
            });
        }

        public void HandleJoinRoomReq(C_JoinRoomReq reqPacket)
        {
            Console.WriteLine($"[HandleJoinRoomReq] Received request for RoomId: {reqPacket.RoomId}");
            GameLogic.Instance.TryEnterRoom(this, reqPacket.RoomId, (result, room) =>
            {
                Console.WriteLine($"[HandleJoinRoomReq] TryEnterRoom result: {result}");
                S_JoinRoomRes resPacket = new S_JoinRoomRes();
                resPacket.Success = result;
                if (result && room != null)
                {
                    resPacket.Room = room.ToRoomDetail();
                }
                Console.WriteLine($"[HandleJoinRoomReq] Sending response packet - Success: {resPacket.Success}");

                var playerInfos = room.GetPlayerInfos();
                Console.WriteLine("\n[Player Details]");
                foreach (var player in playerInfos)
                {
                    Console.WriteLine($"플레이어 ID: {player.PlayerId}" +
                        $"\n\t이름: {player.PlayerName}" +
                        $"\n\t호스트 여부: {player.IsHost}" +
                        $"\n\t팀: {player.Team}" +
                        $"\n\t슬롯 번호: {player.SlotNumber}" +
                        "\n----------------------------------------");
                }
                Send(resPacket);
            });
        }

        public void HandleRoomChat(C_RoomChat reqPacket)
        {
            GameRoom currentGameRoom = Player.GameRoom;

            // 로그: 채팅 요청 받음
            Console.WriteLine($"[HandleRoomChat] Received chat request from Player {Player.PlayerId} ({Player.PlayerName})");
            Console.WriteLine($"[HandleRoomChat] Message: {reqPacket.Message}");

            if (currentGameRoom == null)
            {
                Console.WriteLine($"[HandleRoomChat] Error: Player {Player.PlayerId} is not in any room");
                return;
            }

            // 로그: 게임룸 정보
            Console.WriteLine($"[HandleRoomChat] GameRoom ID: {currentGameRoom.GameRoomId}");

            currentGameRoom.Push(() =>
            {
                Console.WriteLine($"[HandleRoomChat] Pushing broadcast task to room {currentGameRoom.GameRoomId}");
                currentGameRoom.BroadcastChat(Player, reqPacket.Message);
            });
        }

        public void HandleStartRoomReq(C_StartGameReq reqPacket)
        {
            GameRoom currentGameRoom = Player.GameRoom;

            string hostAddress = reqPacket.HostAddress;
            int hostPort = reqPacket.Port;

            // 로그: 게임 시작 요청
            Console.WriteLine($"[HandleStartRoomReq] Received game start request from Player {Player.PlayerId} ({Player.PlayerName})");

            if (currentGameRoom == null)
            {
                Console.WriteLine($"[HandleStartRoomReq] Error: Player {Player.PlayerId} is not in any room");
                return;
            }

            // 로그: 게임룸 정보
            Console.WriteLine($"[HandleStartRoomReq] GameRoom ID: {currentGameRoom.GameRoomId}");

            currentGameRoom.Push(() =>
            {
                Console.WriteLine($"[HandleStartRoomReq] Pushing game start task to room {currentGameRoom.GameRoomId}");
                currentGameRoom.StartGame(hostAddress, hostPort);
            });
        }

        public void HandleLeaveRoomReq(C_LeaveRoomReq reqPacket)
        {
            Console.WriteLine($"[HandleLeaveRoomReq] Player {Player.PlayerId} ({Player.PlayerName}) requested to leave room");

            if (Player.GameRoom == null)
            {
                Console.WriteLine($"[HandleLeaveRoomReq] Error: Player {Player.PlayerId} is not in any room");
                S_LeaveRoomRes resPacket = new S_LeaveRoomRes { Success = false };
                Send(resPacket);
                return;
            }

            GameRoom room = Player.GameRoom;
            Console.WriteLine($"[HandleLeaveRoomReq] Processing leave request - Room ID: {room.GameRoomId}, Current Players: {room.CurrentPlayerCount}");

            room.Push(() =>
            {
                Console.WriteLine($"[HandleLeaveRoomReq] Executing leave room task for Player {Player.PlayerId}");
                room.LeaveRoom(this, (success) =>
                {
                    Console.WriteLine($"[HandleLeaveRoomReq] Leave room result - Success: {success}");
                    S_LeaveRoomRes resPacket = new S_LeaveRoomRes { Success = success };
                    Send(resPacket);
                    Console.WriteLine($"[HandleLeaveRoomReq] Response packet sent to Player {Player.PlayerId}");
                });
            });
        }

        // GameRoom.cs의 BroadcastLeaveGame 구현
        public void BroadcastLeaveGame(Player player)
        {
            Console.WriteLine($"[BroadcastLeaveGame] Broadcasting player leave - Player {player.PlayerId}");

            S_BroadcastLeaveRoom leavePacket = new S_BroadcastLeaveRoom
            {
                Room = player.GameRoom.ToRoomDetail()
            };

            foreach (Player p in player.GameRoom.Players)
            {
                if (p.Session != null)
                {
                    Console.WriteLine($"[BroadcastLeaveGame] Sending to player {p.PlayerId}");
                    p.Session.Send(leavePacket);
                }
            }

            // 방을 나간 플레이어의 정보 초기화
            player.GameRoom = null;
            player.RoomId = 0;
            player.IsHost = false;
            player.SlotNumber = -1;
            player.Team = ETeamType.TeamNone;
        }

    }
}

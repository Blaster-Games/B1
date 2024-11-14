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

            Console.WriteLine("접속 유저 ID : " + reqPacket.AccountDbId);

            S_AuthRes resPacket = new S_AuthRes();
            resPacket.Success = true;

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

        public void HandleJoinRoomReq(C_JoinRoomReq reqPacket)
        {
            Console.WriteLine($"[HandleJoinRoomReq] Received request for RoomId: {reqPacket.RoomId}");

            GameLogic.Instance.TryEnterRoom(this, reqPacket.RoomId, (result) =>
            {
                Console.WriteLine($"[HandleJoinRoomReq] TryEnterRoom result: {result}");

                S_JoinRoomRes resPacket = new S_JoinRoomRes();
                resPacket.Success = result;

                if (result)
                {
                    GameRoom room = GameLogic.Instance.FindRoom(reqPacket.RoomId);
                    Console.WriteLine($"[HandleJoinRoomReq] Found room: {(room != null ? "Yes" : "No")}");

                    if (room != null)
                    {
                        resPacket.Room = room.ToRoomDetail();
                        Console.WriteLine($"[HandleJoinRoomReq] Room Details:" +
                            $"\n\tRoom ID: {resPacket.Room.RoomId}" +
                            $"\n\tRoom Name: {resPacket.Room.RoomName}" +
                            $"\n\tGame Mode: {resPacket.Room.RoomType}" +
                            $"\n\tMax Players: {resPacket.Room.MaxPlayers}" +
                            $"\n\tRoom State: {resPacket.Room.State}" +
                            $"\n\tMap Name: {resPacket.Room.MapName}" +
                            $"\n\tHost Player ID: {resPacket.Room.HostPlayerId}" +
                            $"\n\tPlayers Count: {resPacket.Room.Players.Count}");

                        // Players 정보도 출력
                        foreach (var player in resPacket.Room.Players)
                        {
                            Console.WriteLine($"\tPlayer Info:" +
                                $"\n\t\tPlayer ID: {player.PlayerId}" +
                                $"\n\t\tPlayer Name: {player.PlayerName}" +
                                $"\n\t\tIs Host: {player.IsHost}" +
                                $"\n\t\tTeam: {player.Team}");
                        }
                    }
                    else
                    {
                        resPacket.Success = false;
                        Console.WriteLine("[HandleJoinRoomReq] Room was found but is null - Setting Success to false");
                    }
                }

                Console.WriteLine($"[HandleJoinRoomReq] Sending response packet - Success: {resPacket.Success}");
                Send(resPacket);
            });
        }

        public void HandleRoomChat(C_RoomChat reqPacket)
        {
            GameRoom currentGameRoom = Player.GameRoom;

            if (currentGameRoom == null)
                return;

            currentGameRoom.Push(() =>
            {
                currentGameRoom.BroadcastChat(Player, reqPacket.Message);
            });
        }
    }
}

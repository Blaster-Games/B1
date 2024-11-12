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
            GameLogic.Instance.TryEnterRoom(this, reqPacket.RoomId, (result) =>
            {
                S_JoinRoomRes resPacket = new S_JoinRoomRes();
                resPacket.Success = result;
                if (result)
                {
                    GameRoom room = GameLogic.Instance.FindRoom(reqPacket.RoomId);
                    if (room != null)
                    {
                        resPacket.Room = room.ToRoomDetail();
                    }
                    else
                    {
                        resPacket.Success = false;
                    }
                }
                Send(resPacket);
            });
        }
    }
}

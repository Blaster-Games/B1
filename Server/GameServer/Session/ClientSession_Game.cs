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
            Console.WriteLine(this.Player.PlayerName + "님이 방을 새로고침 했습니다.");

            List<RoomListItemInfo> roomInfoList = GameLogic.Instance.GetRoomListItems();

            S_RoomListRes resPacket = new S_RoomListRes();
            resPacket.Rooms.AddRange(roomInfoList);
            Send(resPacket);
        }
    }
}

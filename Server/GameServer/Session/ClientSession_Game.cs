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

            Console.WriteLine("접속 유저 ID : " + reqPacket.AccountDbId);

            S_AuthRes resPacket = new S_AuthRes();
            resPacket.Success = true;

            Send(resPacket);
        }

        public void HandleEnterLobbyReq(C_EnterLobbyReq reqPacket)
        {
            S_EnterLobbyRes resPacket = new S_EnterLobbyRes();
            Send(resPacket);
        }
    }
}

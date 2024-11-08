using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Google.Protobuf;
using Google.Protobuf.Protocol;
using ServerCore;

namespace GameServer
{
    public class PacketHandler
    {
        public static void C_AuthReqHandler(PacketSession session, IMessage packet)
        {
            C_AuthReq reqPacket = (C_AuthReq)packet;
            ClientSession clientSession = (ClientSession)session;
            clientSession.HandleAuthReq(reqPacket);
        }

        public static void C_CreateRoomReqHandler(PacketSession session, IMessage packet)
        {
            throw new NotImplementedException();
        }

        public static void C_EndGameReqHandler(PacketSession session, IMessage packet)
        {
            throw new NotImplementedException();
        }

        public static void C_EnterLobbyReqHandler(PacketSession session, IMessage packet)
        {
            C_EnterLobbyReq reqPacket = (C_EnterLobbyReq)packet;
            ClientSession clientSession = (ClientSession)session;
            clientSession.HandleEnterLobbyReq(reqPacket);
        }

        public static void C_JoinRoomReqHandler(PacketSession session, IMessage packet)
        {
            throw new NotImplementedException();
        }

        public static void C_LeaveRoomReqHandler(PacketSession session, IMessage packet)
        {
            throw new NotImplementedException();
        }

        public static void C_LobbyChatHandler(PacketSession session, IMessage packet)
        {
            throw new NotImplementedException();
        }

        public static void C_ReadyGameReqHandler(PacketSession session, IMessage packet)
        {
            throw new NotImplementedException();
        }

        public static void C_RoomChatHandler(PacketSession session, IMessage packet)
        {
            throw new NotImplementedException();
        }

        public static void C_RoomListReqHandler(PacketSession session, IMessage packet)
        {
            throw new NotImplementedException();
        }

        public static void C_SelectRoomReqHandler(PacketSession session, IMessage packet)
        {
            throw new NotImplementedException();
        }

        public static void C_StartGameReqHandler(PacketSession session, IMessage packet)
        {
            throw new NotImplementedException();
        }

        public static void C_PongHandler(PacketSession session, IMessage packet)
        {
            ClientSession clientSession = (ClientSession)session;
            clientSession.HandlePong();
        }
    }
}

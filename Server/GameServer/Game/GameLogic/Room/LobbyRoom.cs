using Google.Protobuf;
using Google.Protobuf.Protocol;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameServer
{
    public class LobbyRoom
    {
        HashSet<ClientSession> _sessions = new HashSet<ClientSession>();

        public void Init()
        {
            _sessions.Clear();
        }

        public void Enter(ClientSession session)
        {
            _sessions.Add(session);
            Console.WriteLine(session.Player.PlayerName + " 님이 로비에 입장했습니다.");

            // 입장 메시지 브로드캐스트
            //S_BroadcastLobbyChat enterMsg = new S_BroadcastLobbyChat()
            //{
            //    PlayerName = session.Nickname,
            //    Message = $"{session.Nickname} has entered the lobby."
            //};
            //Broadcast(enterMsg);
        }

        public void Leave(ClientSession session)
        {
            if (_sessions.Remove(session))
            {
                //S_BroadcastLobbyChat leaveMsg = new S_BroadcastLobbyChat()
                //{
                //    PlayerName = session.Nickname,
                //    Message = $"{session.Nickname} has left the lobby."
                //};
                //Broadcast(leaveMsg);
            }
        }

        public void HandleChat(ClientSession session, C_LobbyChat chatPacket)
        {
            // 채팅 쿨타임 체크
            //if ((DateTime.Now - session.LastChatTime).TotalSeconds < 1)
            //    return;

            //session.LastChatTime = DateTime.Now;

            string chatMessage = $"{session.Player.PlayerName}: {chatPacket.Message}";

            // 브로드캐스트
            //S_BroadcastLobbyChat resPacket = new S_BroadcastLobbyChat()
            //{
            //    PlayerName = session.Nickname,
            //    Message = chatPacket.Message
            //};
            //Broadcast(resPacket);
        }

        private void Broadcast(IMessage packet)
        {
            foreach (ClientSession session in _sessions)
                session.Send(packet);
        }
    }
}

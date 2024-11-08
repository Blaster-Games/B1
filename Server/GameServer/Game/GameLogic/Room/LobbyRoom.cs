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
        Queue<string> _chatHistory = new Queue<string>();
        const int MAX_CHAT_HISTORY = 20;

        public void Init()
        {
            _sessions.Clear();
            _chatHistory.Clear();
        }

        public void Enter(ClientSession session)
        {
            _sessions.Add(session);

            // 입장 메시지 브로드캐스트
            S_BroadcastLobbyChat enterMsg = new S_BroadcastLobbyChat()
            {
                PlayerName = session.Nickname,
                Message = $"{session.Nickname} has entered the lobby."
            };
            Broadcast(enterMsg);

            // 채팅 히스토리 전송
            foreach (var chat in _chatHistory)
            {
                S_BroadcastLobbyChat historyMsg = new S_BroadcastLobbyChat()
                {
                    PlayerName = session.Nickname,
                    Message = chat
                };
                session.Send(historyMsg);
            }
        }

        public void Leave(ClientSession session)
        {
            if (_sessions.Remove(session))
            {
                S_BroadcastLobbyChat leaveMsg = new S_BroadcastLobbyChat()
                {
                    PlayerName = session.Nickname,
                    Message = $"{session.Nickname} has left the lobby."
                };
                Broadcast(leaveMsg);
            }
        }

        public void HandleChat(ClientSession session, C_LobbyChat chatPacket)
        {
            // 채팅 쿨타임 체크
            //if ((DateTime.Now - session.LastChatTime).TotalSeconds < 1)
            //    return;

            //session.LastChatTime = DateTime.Now;

            string chatMessage = $"{session.Nickname}: {chatPacket.Message}";

            // 채팅 히스토리 저장
            _chatHistory.Enqueue(chatMessage);
            while (_chatHistory.Count > MAX_CHAT_HISTORY)
                _chatHistory.Dequeue();

            // 브로드캐스트
            S_BroadcastLobbyChat resPacket = new S_BroadcastLobbyChat()
            {
                PlayerName = session.Nickname,
                Message = chatPacket.Message
            };
            Broadcast(resPacket);
        }

        private void Broadcast(IMessage packet)
        {
            foreach (ClientSession session in _sessions)
                session.Send(packet);
        }
    }
}

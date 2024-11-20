using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Google.Protobuf.Protocol;

namespace GameServer
{

    public class Player
    {
        public ClientSession Session { get; set; }
        public int PlayerId => Session.SessionId;
        public string PlayerName { get; set; }
        public bool IsHost { get; set; } = false;
        public ETeamType Team { get; set; } = ETeamType.TeamNone;
        public int RoomId { get; set; }
        public GameRoom GameRoom { get; set; }
        public int SlotNumber { get; set; } = -1;

        public Player(ClientSession session)
        {
            Session = session;
        }
    }
}

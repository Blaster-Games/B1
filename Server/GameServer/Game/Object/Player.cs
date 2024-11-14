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
        public int PlayerId { get; set; }
        public string PlayerName { get; set; }
        public bool IsHost { get; set; }
        public ETeamType Team { get; set; }

        // 현재 있는 방 정보 (방에 있을 경우)
        public int RoomId { get; set; }
        public GameRoom GameRoom { get; set; }
    }
}

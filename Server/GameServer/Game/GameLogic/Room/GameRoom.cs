using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameServer
{
    public class GameRoom : JobSerializer
    {
        public int GameRoomId { get; set; }

        public void Init()
        {

        }

        public void AddPlayers(List<Player> players)
        {
            throw new NotImplementedException();
        }
    }
}

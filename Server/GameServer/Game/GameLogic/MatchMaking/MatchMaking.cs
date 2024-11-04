using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace GameServer
{
    public class MatchMaking : JobSerializer
    {
        private List<Player> _waitingPlayers = new List<Player>();
        private const int PLAYERS_PER_MATCH = 8; // 4vs4

        public void EnqueuePlayer(Player player)
        {
            Push(() =>
            {
                _waitingPlayers.Add(player);
                TryCreateMatch();
            });
        }

        private void TryCreateMatch()
        {
            if (_waitingPlayers.Count >= PLAYERS_PER_MATCH)
            {
                List<Player> matchedPlayers = _waitingPlayers.Take(PLAYERS_PER_MATCH).ToList();
                _waitingPlayers.RemoveRange(0, PLAYERS_PER_MATCH);

                GameLogic.Instance.Push(() => CreateNewGame(matchedPlayers));
            }
        }

        private void CreateNewGame(List<Player> players)
        {
            GameRoom newRoom = GameLogic.Add();
            newRoom.AddPlayers(players);
            GameLogic._updateQueue.Enqueue(newRoom);
        }

        public void Update()
        {
            Flush();
        }

        // 매칭 취소 로직
        public void CancelMatch(Player player)
        {
            Push(() =>
            {
                _waitingPlayers.Remove(player);
            });
        }
    }
}

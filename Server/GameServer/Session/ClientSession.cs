using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using Google.Protobuf;
using Google.Protobuf.Protocol;
using ServerCore;

namespace GameServer
{
    public partial class ClientSession : PacketSession
    {
        public Player Player { get; set; }
        public long AccountDbId { get; set; }
        public int SessionId { get; set; }
        public string AccessToken { get; set; }
        public string IpAddress { get; set; }

        long _pingpongTick = 0;

        public ClientSession() 
        {
            Player = new Player(this);
        }

        public void Ping()
        {
            if (_pingpongTick > 0)
            {
                long delta = (System.Environment.TickCount64 - _pingpongTick);
                Console.WriteLine($"Ping check - Last pong: {delta}ms ago");

                if (delta > 30 * 1000)  // 30초
                {
                    Console.WriteLine($"Disconnected by PingCheck - No response for {delta}ms");
                    Disconnect();
                    return;
                }
            }

            S_Ping pingPacket = new S_Ping();
            Send(pingPacket);
            Console.WriteLine("Sent Ping packet");

            GameLogic.Instance.PushAfter(5000, Ping);  // 10초
        }

        public void HandlePong()
        {
            _pingpongTick = System.Environment.TickCount64;
        }

        #region Network

        public void Send(IMessage packet)
        {
            Send(new ArraySegment<byte>(MakeSendBuffer(packet)));
        }

        // header 포함 전체 size (2byte) + packet id (2byte) + packet body (xbyte) 구조로 sendBuffer에 copy
        public static byte[] MakeSendBuffer(IMessage packet)
        {
            MsgId msgId = (MsgId)Enum.Parse(typeof(MsgId), packet.Descriptor.Name);
            ushort size = (ushort)packet.CalculateSize();
            byte[] sendBuffer = new byte[size + 4];
            Array.Copy(BitConverter.GetBytes((ushort)(size + 4)), 0, sendBuffer, 0, sizeof(ushort));
            Array.Copy(BitConverter.GetBytes((ushort)msgId), 0, sendBuffer, 2, sizeof(ushort));
            Array.Copy(packet.ToByteArray(), 0, sendBuffer, 4, size);
            return sendBuffer;
        }

        public override void OnConnected(EndPoint endPoint)
        {
            if (endPoint is IPEndPoint ipEndPoint)
            {
                IpAddress = ipEndPoint.Address.ToString();
                Console.WriteLine($"OnConnected : {IpAddress}");
            }

            S_Connected resPacket = new S_Connected();

            Send(resPacket);

            //GameLogic.Instance.PushAfter(5000, Ping);
        }

        public override async void OnDisconnected(EndPoint endPoint)
        {
            Console.WriteLine($"OnDisconnected Start: {endPoint}");  // 시작 로그

            if (Player.GameRoom != null)
            {
                Player.GameRoom.LeaveRoom(this);
                Console.WriteLine($"Player {Player.PlayerName} has left the room");
            }

            // GameRoom 체크와 관계없이 로그아웃 요청을 보내도록 수정
            Console.WriteLine($"Attempting to send logout request..."); // 로그아웃 시도 로그
            Console.WriteLine($"AccessToken value: {AccessToken}");

            try
            {
                using (var client = new HttpClient())
                {
                    client.DefaultRequestHeaders.Add("Authorization", "Bearer " + AccessToken);
                    Console.WriteLine("Headers set: " + string.Join(", ", client.DefaultRequestHeaders.Select(h => $"{h.Key}: {string.Join(", ", h.Value)}")));

                    var url = "https://native-pika-possibly.ngrok-free.app/api/member/logout/game";
                    Console.WriteLine($"Sending request to: {url}");

                    var response = await client.PostAsync(url, null);

                    Console.WriteLine($"Response Status: {response.StatusCode}");
                    var responseBody = await response.Content.ReadAsStringAsync();
                    Console.WriteLine($"Response Body: {responseBody}");

                    if (response.IsSuccessStatusCode)
                        Console.WriteLine($"Logout request sent successfully for player {Player.PlayerName}");
                    else
                        Console.WriteLine($"Logout request failed: {response.StatusCode} - {responseBody}");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error sending logout request: {ex.Message}");
                Console.WriteLine($"Stack trace: {ex.StackTrace}");
                if (ex.InnerException != null)
                {
                    Console.WriteLine($"Inner exception: {ex.InnerException.Message}");
                }
            }

            Console.WriteLine($"OnDisconnected End: {endPoint}");  // 종료 로그
        }

        public override void OnRecvPacket(ArraySegment<byte> buffer)
        {
            Console.WriteLine($"Recv Packet Data : {buffer}");
            PacketManager.Instance.OnRecvPacket(this, buffer);
        }

        public override void OnSend(int numOfBytes)
        {
            Console.WriteLine($"Transferred bytes: {numOfBytes}");
        }
        #endregion
    }
}

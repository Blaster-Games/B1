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

        long _pingpongTick = 0;

        public ClientSession() 
        { 
            Player = new Player();
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
            Console.WriteLine($"OnConnected : {endPoint}");

            S_Connected resPacket = new S_Connected();

            Send(resPacket);

            GameLogic.Instance.PushAfter(5000, Ping);
        }

        public override void OnDisconnected(EndPoint endPoint)
        {
            Console.WriteLine($"OnDisconnected : {endPoint}");
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

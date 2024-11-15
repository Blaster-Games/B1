using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GameServer;
using Google.Protobuf;
using Google.Protobuf.Protocol;
using ServerCore;

public enum MsgId
{
    // 기본 연결
    S_Connected = 1001,

    // 인증
    C_AuthReq = 1002,          // 클라 -> 서버: JWT 전송
    S_AuthRes = 1003,          // 서버 -> 클라: 인증 결과

    // 로비
    C_EnterLobbyReq = 1004,    // 클라 -> 서버: 로비 입장 요청
    S_EnterLobbyRes = 1005,    // 서버 -> 클라: 로비 정보 (룸 목록)
    C_LobbyChat = 1006,        // 클라 -> 서버: 로비 채팅
    S_BroadcastLobbyChat = 1007, // 서버 -> 모든 로비 유저: 채팅 브로드캐스트

    // 룸 목록
    C_RoomListReq = 1008,      // 클라 -> 서버: 룸 목록 갱신 요청
    S_RoomListRes = 1009,      // 서버 -> 클라: 최신 룸 목록

    // 룸 선택/상세정보
    C_SelectRoomReq = 1010,    // 클라 -> 서버: 특정 룸 정보 요청
    S_SelectRoomRes = 1011,    // 서버 -> 클라: 룸 상세 정보

    // 룸 생성
    C_CreateRoomReq = 1012,    // 클라 -> 서버: 새 룸 생성 요청
    S_CreateRoomRes = 1013,    // 서버 -> 클라: 룸 생성 결과

    // 룸 입장
    C_JoinRoomReq = 1014,      // 클라 -> 서버: 룸 입장 요청
    S_JoinRoomRes = 1015,      // 서버 -> 클라: 룸 입장 결과 + 전체 룸 정보
    S_BroadcastJoinRoom = 1016, // 서버 -> 룸 유저들: 새 플레이어 입장 알림

    // 룸 퇴장
    C_LeaveRoomReq = 1017,     // 클라 -> 서버: 룸 퇴장 요청
    S_LeaveRoomRes = 1018,     // 서버 -> 클라: 퇴장 결과
    S_BroadcastLeaveRoom = 1019, // 서버 -> 룸 유저들: 플레이어 퇴장 알림

    // 룸 채팅
    C_RoomChat = 1020,         // 클라 -> 서버: 룸 채팅
    S_BroadcastRoomChat = 1021, // 서버 -> 룸 유저들: 채팅 브로드캐스트

    // 게임 시작
    C_StartGameReq = 1022,     // 호스트 -> 서버: 리슨서버 정보 전송
    S_StartGameRes = 1023,     // 서버 -> 호스트: 다른 플레이어 정보 전달
    S_BroadcastStartGame = 1024, // 서버 -> 룸 유저들: 호스트 리슨서버 접속 정보

    // 게임 준비
    C_ReadyGameReq = 1025,     // 클라 -> 서버: 게임 준비 상태 변경
    S_BroadcastReadyGame = 1026, // 서버 -> 룸 유저들: 플레이어 준비 상태 변경

    // 게임 종료
    C_EndGameReq = 1027,       // 호스트 -> 서버: 게임 종료 알림
    S_EndGameRes = 1028,       // 서버 -> 호스트: 종료 처리 완료
    S_BroadcastEndGame = 1029, // 서버 -> 룸 유저들: 게임 종료 알림

    // 호스트 변경
    S_BroadcastChangeHost = 1030, // 서버 -> 룸 유저들: 새로운 호스트 지정

    // 룸 상태 변경
    S_BroadcastRoomState = 1031, // 서버 -> 룸 유저들: 룸 상태 변경 (게임중, 대기중 등)

    // 에러/킥
    S_ErrorMessage = 1032,     // 서버 -> 클라: 에러 메시지
    S_KickPlayer = 1033,       // 서버 -> 클라: 강제 퇴장

    // 하트비트
    S_Ping = 1034,
    C_Pong = 1035
}

class PacketManager
{
    #region Singleton
    static PacketManager _instance = new PacketManager();
    public static PacketManager Instance { get { return _instance; } }
    #endregion

    PacketManager()
    {
        Register();
    }

    Dictionary<ushort, Action<PacketSession, ArraySegment<byte>, ushort>> _onRecv = new Dictionary<ushort, Action<PacketSession, ArraySegment<byte>, ushort>>();
    Dictionary<ushort, Action<PacketSession, IMessage>> _handler = new Dictionary<ushort, Action<PacketSession, IMessage>>();

    public Action<PacketSession, IMessage, ushort> CustomHandler { get; set; }

    public void Register()
    {
        // 인증
        _onRecv.Add((ushort)MsgId.C_AuthReq, MakePacket<C_AuthReq>);
        _handler.Add((ushort)MsgId.C_AuthReq, PacketHandler.C_AuthReqHandler);

        // 로비 관련
        _onRecv.Add((ushort)MsgId.C_EnterLobbyReq, MakePacket<C_EnterLobbyReq>);
        _handler.Add((ushort)MsgId.C_EnterLobbyReq, PacketHandler.C_EnterLobbyReqHandler);

        _onRecv.Add((ushort)MsgId.C_LobbyChat, MakePacket<C_LobbyChat>);
        _handler.Add((ushort)MsgId.C_LobbyChat, PacketHandler.C_LobbyChatHandler);

        // 룸 목록
        _onRecv.Add((ushort)MsgId.C_RoomListReq, MakePacket<C_RoomListReq>);
        _handler.Add((ushort)MsgId.C_RoomListReq, PacketHandler.C_RoomListReqHandler);

        // 룸 선택/상세정보
        _onRecv.Add((ushort)MsgId.C_SelectRoomReq, MakePacket<C_SelectRoomReq>);
        _handler.Add((ushort)MsgId.C_SelectRoomReq, PacketHandler.C_SelectRoomReqHandler);

        // 룸 생성
        _onRecv.Add((ushort)MsgId.C_CreateRoomReq, MakePacket<C_CreateRoomReq>);
        _handler.Add((ushort)MsgId.C_CreateRoomReq, PacketHandler.C_CreateRoomReqHandler);

        // 룸 입장
        _onRecv.Add((ushort)MsgId.C_JoinRoomReq, MakePacket<C_JoinRoomReq>);
        _handler.Add((ushort)MsgId.C_JoinRoomReq, PacketHandler.C_JoinRoomReqHandler);

        // 룸 퇴장
        _onRecv.Add((ushort)MsgId.C_LeaveRoomReq, MakePacket<C_LeaveRoomReq>);
        _handler.Add((ushort)MsgId.C_LeaveRoomReq, PacketHandler.C_LeaveRoomReqHandler);

        // 룸 채팅
        _onRecv.Add((ushort)MsgId.C_RoomChat, MakePacket<C_RoomChat>);
        _handler.Add((ushort)MsgId.C_RoomChat, PacketHandler.C_RoomChatHandler);

        // 게임 시작
        _onRecv.Add((ushort)MsgId.C_StartGameReq, MakePacket<C_StartGameReq>);
        _handler.Add((ushort)MsgId.C_StartGameReq, PacketHandler.C_StartGameReqHandler);

        // 게임 준비
        _onRecv.Add((ushort)MsgId.C_ReadyGameReq, MakePacket<C_ReadyGameReq>);
        _handler.Add((ushort)MsgId.C_ReadyGameReq, PacketHandler.C_ReadyGameReqHandler);

        // 게임 종료
        _onRecv.Add((ushort)MsgId.C_EndGameReq, MakePacket<C_EndGameReq>);
        _handler.Add((ushort)MsgId.C_EndGameReq, PacketHandler.C_EndGameReqHandler);

        // 하트비트
        _onRecv.Add((ushort)MsgId.C_Pong, MakePacket<C_Pong>);
        _handler.Add((ushort)MsgId.C_Pong, PacketHandler.C_PongHandler);
    }

    public void OnRecvPacket(PacketSession session, ArraySegment<byte> buffer)
    {
        ushort count = 0;

        ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
        count += 2;
        ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + count);
        count += 2;

        Action<PacketSession, ArraySegment<byte>, ushort> action = null;
        if (_onRecv.TryGetValue(id, out action))
            action.Invoke(session, buffer, id);
    }

    void MakePacket<T>(PacketSession session, ArraySegment<byte> buffer, ushort id) where T : IMessage, new()
    {
        T pkt = new T();
        pkt.MergeFrom(buffer.Array, buffer.Offset + 4, buffer.Count - 4);

        if (CustomHandler != null)
        {
            CustomHandler.Invoke(session, pkt, id);
        }
        else
        {
            Action<PacketSession, IMessage> action = null;
            if (_handler.TryGetValue(id, out action))
                action.Invoke(session, pkt);
        }
    }
}

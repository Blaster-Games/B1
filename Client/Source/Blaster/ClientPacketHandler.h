#pragma once
#include "Protocol.pb.h"
#include "Blaster.h"

#include <functional>

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
    // 기본 연결
    PKT_S_Connected = 1001,

    // 인증
    PKT_C_AuthReq = 1002,
    PKT_S_AuthRes = 1003,

    // 로비
    PKT_C_EnterLobbyReq = 1004,
    PKT_S_EnterLobbyRes = 1005,
    PKT_C_LobbyChat = 1006,
    PKT_S_BroadcastLobbyChat = 1007,

    // 룸 목록
    PKT_C_RoomListReq = 1008,
    PKT_S_RoomListRes = 1009,

    // 룸 선택/상세정보
    PKT_C_SelectRoomReq = 1010,
    PKT_S_SelectRoomRes = 1011,

    // 룸 생성
    PKT_C_CreateRoomReq = 1012,
    PKT_S_CreateRoomRes = 1013,

    // 룸 입장
    PKT_C_JoinRoomReq = 1014,
    PKT_S_JoinRoomRes = 1015,
    PKT_S_BroadcastJoinRoom = 1016,

    // 룸 퇴장
    PKT_C_LeaveRoomReq = 1017,
    PKT_S_LeaveRoomRes = 1018,
    PKT_S_BroadcastLeaveRoom = 1019,

    // 룸 채팅
    PKT_C_RoomChat = 1020,
    PKT_S_BroadcastRoomChat = 1021,

    // 게임 시작
    PKT_C_StartGameReq = 1022,
    PKT_S_StartGameRes = 1023,
    PKT_S_BroadcastStartGame = 1024,

    // 게임 준비
    PKT_C_ReadyGameReq = 1025,
    PKT_S_BroadcastReadyGame = 1026,

    // 게임 종료
    PKT_C_EndGameReq = 1027,
    PKT_S_EndGameRes = 1028,
    PKT_S_BroadcastEndGame = 1029,

    // 호스트 변경
    PKT_S_BroadcastChangeHost = 1030,

    // 룸 상태 변경
    PKT_S_BroadcastRoomState = 1031,

    // 에러/킥
    PKT_S_ErrorMessage = 1032,
    PKT_S_KickPlayer = 1033,

    // 하트비트
    PKT_S_Ping = 1034,
    PKT_C_Pong = 1035
};

// Custom Handlers
bool Handle_Invalid(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_Connected(PacketSessionRef& session, Protocol::S_Connected& pkt);
bool Handle_S_AuthRes(PacketSessionRef& session, Protocol::S_AuthRes& pkt);
bool Handle_S_EnterLobbyRes(PacketSessionRef& session, Protocol::S_EnterLobbyRes& pkt);
bool Handle_S_BroadcastLobbyChat(PacketSessionRef& session, Protocol::S_BroadcastLobbyChat& pkt);
bool Handle_S_RoomListRes(PacketSessionRef& session, Protocol::S_RoomListRes& pkt);
bool Handle_S_SelectRoomRes(PacketSessionRef& session, Protocol::S_SelectRoomRes& pkt);
bool Handle_S_CreateRoomRes(PacketSessionRef& session, Protocol::S_CreateRoomRes& pkt);
bool Handle_S_JoinRoomRes(PacketSessionRef& session, Protocol::S_JoinRoomRes& pkt);
bool Handle_S_BroadcastJoinRoom(PacketSessionRef& session, Protocol::S_BroadcastJoinRoom& pkt);
bool Handle_S_LeaveRoomRes(PacketSessionRef& session, Protocol::S_LeaveRoomRes& pkt);
bool Handle_S_BroadcastLeaveRoom(PacketSessionRef& session, Protocol::S_BroadcastLeaveRoom& pkt);
bool Handle_S_BroadcastRoomChat(PacketSessionRef& session, Protocol::S_BroadcastRoomChat& pkt);
bool Handle_S_StartGameRes(PacketSessionRef& session, Protocol::S_StartGameRes& pkt);
bool Handle_S_BroadcastStartGame(PacketSessionRef& session, Protocol::S_BroadcastStartGame& pkt);
bool Handle_S_BroadcastReadyGame(PacketSessionRef& session, Protocol::S_BroadcastReadyGame& pkt);
bool Handle_S_EndGameRes(PacketSessionRef& session, Protocol::S_EndGameRes& pkt);
bool Handle_S_BroadcastEndGame(PacketSessionRef& session, Protocol::S_BroadcastEndGame& pkt);
bool Handle_S_BroadcastChangeHost(PacketSessionRef& session, Protocol::S_BroadcastChangeHost& pkt);
bool Handle_S_BroadcastRoomState(PacketSessionRef& session, Protocol::S_BroadcastRoomState& pkt);
bool Handle_S_ErrorMessage(PacketSessionRef& session, Protocol::S_ErrorMessage& pkt);
bool Handle_S_KickPlayer(PacketSessionRef& session, Protocol::S_KickPlayer& pkt);
bool Handle_S_Ping(PacketSessionRef& session, Protocol::S_Ping& pkt);

class ClientPacketHandler
{
public:
    static void Init()
    {
        for (int32 i = 0; i < UINT16_MAX; i++)
            GPacketHandler[i] = Handle_Invalid;

        GPacketHandler[PKT_S_Connected] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_Connected>(Handle_S_Connected, session, buffer, len); };
        GPacketHandler[PKT_S_AuthRes] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_AuthRes>(Handle_S_AuthRes, session, buffer, len); };
        GPacketHandler[PKT_S_EnterLobbyRes] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_EnterLobbyRes>(Handle_S_EnterLobbyRes, session, buffer, len); };
        GPacketHandler[PKT_S_BroadcastLobbyChat] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_BroadcastLobbyChat>(Handle_S_BroadcastLobbyChat, session, buffer, len); };
        GPacketHandler[PKT_S_RoomListRes] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_RoomListRes>(Handle_S_RoomListRes, session, buffer, len); };
        GPacketHandler[PKT_S_SelectRoomRes] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SelectRoomRes>(Handle_S_SelectRoomRes, session, buffer, len); };
        GPacketHandler[PKT_S_CreateRoomRes] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CreateRoomRes>(Handle_S_CreateRoomRes, session, buffer, len); };
        GPacketHandler[PKT_S_JoinRoomRes] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_JoinRoomRes>(Handle_S_JoinRoomRes, session, buffer, len); };
        GPacketHandler[PKT_S_BroadcastJoinRoom] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_BroadcastJoinRoom>(Handle_S_BroadcastJoinRoom, session, buffer, len); };
        GPacketHandler[PKT_S_LeaveRoomRes] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LeaveRoomRes>(Handle_S_LeaveRoomRes, session, buffer, len); };
        GPacketHandler[PKT_S_BroadcastLeaveRoom] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_BroadcastLeaveRoom>(Handle_S_BroadcastLeaveRoom, session, buffer, len); };
        GPacketHandler[PKT_S_BroadcastRoomChat] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_BroadcastRoomChat>(Handle_S_BroadcastRoomChat, session, buffer, len); };
        GPacketHandler[PKT_S_StartGameRes] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_StartGameRes>(Handle_S_StartGameRes, session, buffer, len); };
        GPacketHandler[PKT_S_BroadcastStartGame] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_BroadcastStartGame>(Handle_S_BroadcastStartGame, session, buffer, len); };
        GPacketHandler[PKT_S_BroadcastReadyGame] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_BroadcastReadyGame>(Handle_S_BroadcastReadyGame, session, buffer, len); };
        GPacketHandler[PKT_S_EndGameRes] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_EndGameRes>(Handle_S_EndGameRes, session, buffer, len); };
        GPacketHandler[PKT_S_BroadcastEndGame] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_BroadcastEndGame>(Handle_S_BroadcastEndGame, session, buffer, len); };
        GPacketHandler[PKT_S_BroadcastChangeHost] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_BroadcastChangeHost>(Handle_S_BroadcastChangeHost, session, buffer, len); };
        GPacketHandler[PKT_S_BroadcastRoomState] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_BroadcastRoomState>(Handle_S_BroadcastRoomState, session, buffer, len); };
        GPacketHandler[PKT_S_ErrorMessage] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ErrorMessage>(Handle_S_ErrorMessage, session, buffer, len); };
        GPacketHandler[PKT_S_KickPlayer] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_KickPlayer>(Handle_S_KickPlayer, session, buffer, len); };
        GPacketHandler[PKT_S_Ping] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_Ping>(Handle_S_Ping, session, buffer, len); };
    }

    static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
    {
        PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

        UE_LOG(LogTemp, Log, TEXT("[PacketHandler] Handling Packet - ID: %d, Length: %d"),
            header->id, len);

        bool result = GPacketHandler[header->id](session, buffer, len);

        UE_LOG(LogTemp, Log, TEXT("[PacketHandler] Packet handling %s - ID: %d"),
            result ? TEXT("succeeded") : TEXT("failed"), header->id);

        return result;
    }

    // C_로 시작하는 패킷들의 MakeSendBuffer 메서드
    static SendBufferRef MakeSendBuffer(Protocol::C_AuthReq& pkt) { return MakeSendBuffer(pkt, PKT_C_AuthReq); }
    static SendBufferRef MakeSendBuffer(Protocol::C_EnterLobbyReq& pkt) { return MakeSendBuffer(pkt, PKT_C_EnterLobbyReq); }
    static SendBufferRef MakeSendBuffer(Protocol::C_LobbyChat& pkt) { return MakeSendBuffer(pkt, PKT_C_LobbyChat); }
    static SendBufferRef MakeSendBuffer(Protocol::C_RoomListReq& pkt) { return MakeSendBuffer(pkt, PKT_C_RoomListReq); }
    static SendBufferRef MakeSendBuffer(Protocol::C_SelectRoomReq& pkt) { return MakeSendBuffer(pkt, PKT_C_SelectRoomReq); }
    static SendBufferRef MakeSendBuffer(Protocol::C_CreateRoomReq& pkt) { return MakeSendBuffer(pkt, PKT_C_CreateRoomReq); }
    static SendBufferRef MakeSendBuffer(Protocol::C_JoinRoomReq& pkt) { return MakeSendBuffer(pkt, PKT_C_JoinRoomReq); }
    static SendBufferRef MakeSendBuffer(Protocol::C_LeaveRoomReq& pkt) { return MakeSendBuffer(pkt, PKT_C_LeaveRoomReq); }
    static SendBufferRef MakeSendBuffer(Protocol::C_RoomChat& pkt) { return MakeSendBuffer(pkt, PKT_C_RoomChat); }
    static SendBufferRef MakeSendBuffer(Protocol::C_StartGameReq& pkt) { return MakeSendBuffer(pkt, PKT_C_StartGameReq); }
    static SendBufferRef MakeSendBuffer(Protocol::C_ReadyGameReq& pkt) { return MakeSendBuffer(pkt, PKT_C_ReadyGameReq); }
    static SendBufferRef MakeSendBuffer(Protocol::C_EndGameReq& pkt) { return MakeSendBuffer(pkt, PKT_C_EndGameReq); }
    static SendBufferRef MakeSendBuffer(Protocol::C_Pong& pkt) { return MakeSendBuffer(pkt, PKT_C_Pong); }

private:
    template<typename PacketType, typename ProcessFunc>
    static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
    {
        UE_LOG(LogTemp, Log, TEXT("[PacketHandler] Attempting to parse packet"));

        PacketType pkt;
        if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
        {
            UE_LOG(LogTemp, Warning, TEXT("[PacketHandler] Failed to parse packet"));
            return false;
        }

        UE_LOG(LogTemp, Log, TEXT("[PacketHandler] Calling handler function for packet"));
        return func(session, pkt);  // Cpp 핸들 메서드 호출
    }

    template<typename T>
    static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
    {
        const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
        const uint16 packetSize = dataSize + sizeof(PacketHeader);

        SendBufferRef sendBuffer = MakeShared<SendBuffer>(packetSize);
        PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
        header->size = packetSize;
        header->id = pktId;
		pkt.SerializeToArray(&header[1], dataSize);
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};
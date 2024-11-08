#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "GameInstance/BlasterGameInstance.h"
#include "GameInstance/BlasterNetworkSubsystem.h"
#include "Network/PacketSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

UBlasterNetworkSubsystem* GetNetworkSystem(const PacketSessionRef& Session)
{
    // 엔진의 모든 월드를 순회
    for (const FWorldContext& Context : GEngine->GetWorldContexts())
    {
        if (UGameInstance* GameInstance = Context.World()->GetGameInstance())
        {
            if (UBlasterNetworkSubsystem* NetworkSystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
            {
                if (NetworkSystem->GameServerSession == Session)
                {
                    return NetworkSystem;
                }
            }
        }
    }
    return nullptr;
}

bool Handle_Invalid(PacketSessionRef& session, BYTE* buffer, int32 len)
{
    return false;
}

bool Handle_S_Connected(PacketSessionRef& session, Protocol::S_Connected& pkt)
{
    return true;
}

bool Handle_S_AuthRes(PacketSessionRef& session, Protocol::S_AuthRes& pkt)
{
    return false;
}

bool Handle_S_EnterLobbyRes(PacketSessionRef& session, Protocol::S_EnterLobbyRes& pkt)
{
    return false;
}

bool Handle_S_BroadcastLobbyChat(PacketSessionRef& session, Protocol::S_BroadcastLobbyChat& pkt)
{
    return false;
}

bool Handle_S_RoomListRes(PacketSessionRef& session, Protocol::S_RoomListRes& pkt)
{
    return false;
}

bool Handle_S_SelectRoomRes(PacketSessionRef& session, Protocol::S_SelectRoomRes& pkt)
{
    return false;
}

bool Handle_S_CreateRoomRes(PacketSessionRef& session, Protocol::S_CreateRoomRes& pkt)
{
    return false;
}

bool Handle_S_JoinRoomRes(PacketSessionRef& session, Protocol::S_JoinRoomRes& pkt)
{
    return false;
}

bool Handle_S_BroadcastJoinRoom(PacketSessionRef& session, Protocol::S_BroadcastJoinRoom& pkt)
{
    return false;
}

bool Handle_S_LeaveRoomRes(PacketSessionRef& session, Protocol::S_LeaveRoomRes& pkt)
{
    return false;
}

bool Handle_S_BroadcastLeaveRoom(PacketSessionRef& session, Protocol::S_BroadcastLeaveRoom& pkt)
{
    return false;
}

bool Handle_S_BroadcastRoomChat(PacketSessionRef& session, Protocol::S_BroadcastRoomChat& pkt)
{
    return false;
}

bool Handle_S_StartGameRes(PacketSessionRef& session, Protocol::S_StartGameRes& pkt)
{
    return false;
}

bool Handle_S_BroadcastStartGame(PacketSessionRef& session, Protocol::S_BroadcastStartGame& pkt)
{
    return false;
}

bool Handle_S_BroadcastReadyGame(PacketSessionRef& session, Protocol::S_BroadcastReadyGame& pkt)
{
    return false;
}

bool Handle_S_EndGameRes(PacketSessionRef& session, Protocol::S_EndGameRes& pkt)
{
    return false;
}

bool Handle_S_BroadcastEndGame(PacketSessionRef& session, Protocol::S_BroadcastEndGame& pkt)
{
    return false;
}

bool Handle_S_BroadcastChangeHost(PacketSessionRef& session, Protocol::S_BroadcastChangeHost& pkt)
{
    return false;
}

bool Handle_S_BroadcastRoomState(PacketSessionRef& session, Protocol::S_BroadcastRoomState& pkt)
{
    return false;
}

bool Handle_S_ErrorMessage(PacketSessionRef& session, Protocol::S_ErrorMessage& pkt)
{
    return false;
}

bool Handle_S_KickPlayer(PacketSessionRef& session, Protocol::S_KickPlayer& pkt)
{
    return false;
}

bool Handle_S_Ping(PacketSessionRef& session, Protocol::S_Ping& pkt)
{
    if (auto NetworkSystem = GetNetworkSystem(session))
    {
        NetworkSystem->HandlePing();
        return true;
    }
    return false;
}

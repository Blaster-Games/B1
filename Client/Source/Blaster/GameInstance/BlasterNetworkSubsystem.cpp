#include "BlasterNetworkSubsystem.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "BlasterGameInstance.h"
#include "HUD/Lobby/RoomList.h"

void UBlasterNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LoadNetworkSettings();
    ClientPacketHandler::Init();
}

void UBlasterNetworkSubsystem::Deinitialize()
{
    DisconnectFromGameServer();
    Super::Deinitialize();
}

void UBlasterNetworkSubsystem::LoadNetworkSettings()
{
    if (UBlasterGameInstance* GameInst = Cast<UBlasterGameInstance>(GetGameInstance()))
    {
        IpAddress = GameInst->DefaultServerIP;
        Port = GameInst->DefaultServerPort;
    }
}

void UBlasterNetworkSubsystem::ConnectToGameServer()
{
    // 이미 연결되어 있다면 연결 해제
    if (Socket)
    {
        DisconnectFromGameServer();
    }

    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));
    if (!Socket)
    {
        GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Failed to create socket"));
        return;
    }

    FIPv4Address Ip;
    FIPv4Address::Parse(IpAddress, Ip);
    TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    InternetAddr->SetIp(Ip.Value);
    InternetAddr->SetPort(Port);

    GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Connecting To Server..."));
    bool Connected = Socket->Connect(*InternetAddr);

    if (Connected)
    {
        GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Connection Success"));

        GameServerSession = MakeShared<PacketSession>(Socket);
        GameServerSession->Run();
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Connection Failed"));
        DisconnectFromGameServer();
    }
}

void UBlasterNetworkSubsystem::DisconnectFromGameServer()
{
    UE_LOG(LogTemp, Warning, TEXT("Disconnected from server"));

    if (Socket)
    {
        ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
        SocketSubsystem->DestroySocket(Socket);
        Socket = nullptr;
    }

    GameServerSession.Reset();
}

void UBlasterNetworkSubsystem::HandleRecvPackets()
{
    if (Socket == nullptr || GameServerSession == nullptr)
    {
        return;
    }

    GameServerSession->HandleRecvPackets();
}

void UBlasterNetworkSubsystem::SendPacket(SendBufferRef SendBuffer)
{
    if (Socket == nullptr || GameServerSession == nullptr)
    {
        return;
    }

    GameServerSession->SendPacket(SendBuffer);
}

void UBlasterNetworkSubsystem::SendAuthReq()
{
    Protocol::C_AuthReq AuthReq;

    // 게임 인스턴스에서 값 가져오기
    if (UBlasterGameInstance* GameInst = Cast<UBlasterGameInstance>(GetGameInstance()))
    {
        AuthReq.set_jwt(TCHAR_TO_UTF8(*GameInst->AccessToken));
        AuthReq.set_accountdbid(GameInst->UserId);
        AuthReq.set_nickname(TCHAR_TO_UTF8(*GameInst->Nickname));

        // 패킷 데이터 검증 로그 추가
        UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] SendAuthReq - JWT: %s, AccountDbId: %d, Nickname: %s"),
            *GameInst->AccessToken, GameInst->UserId, *GameInst->Nickname);

        // Protocol Buffer 객체 검증
        UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] Protocol Buffer - Nickname set: %s"),
            UTF8_TO_TCHAR(AuthReq.nickname().c_str()));

        // 화면에도 디버그 메시지 표시 (선택사항)
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
            FString::Printf(TEXT("Sending Auth - ID: %d : Nickname: %s"),
                GameInst->UserId,
                *GameInst->Nickname));
    }
    else
    {
        // 게임 인스턴스를 가져오지 못했을 경우 에러 로그
        UE_LOG(LogTemp, Error, TEXT("[NetworkSubsystem] Failed to get BlasterGameInstance"));
        return;
    }

    SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(AuthReq);
    SendPacket(SendBuffer);
}

void UBlasterNetworkSubsystem::HandleAuthRes(Protocol::S_AuthRes& packet)
{
    UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] Auth response received"));

    if (packet.success())
    {
        UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] Auth successful"));
        OnAuthSuccess.Broadcast();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[NetworkSubsystem] Auth failed"));
        OnAuthFailed.Broadcast(TEXT("인증에 실패했습니다."));
    }
}

void UBlasterNetworkSubsystem::SendEnterLobbyReq()
{
    Protocol::C_EnterLobbyReq EnterLobbyReq;

    SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(EnterLobbyReq);
    SendPacket(SendBuffer);
}

void UBlasterNetworkSubsystem::HandleEnterLobbyRes(Protocol::S_EnterLobbyRes& packet)
{
    OnEnterLobbyResponse.Broadcast();
}

void UBlasterNetworkSubsystem::SendRoomListReq()
{
    Protocol::C_RoomListReq RoomListReq;

    SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(RoomListReq);
    SendPacket(SendBuffer);
}

void UBlasterNetworkSubsystem::HandleRoomListRes(Protocol::S_RoomListRes& packet)
{
    // 패킷으로 받은 전체 방 개수 로깅
    UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] Received room list - Total rooms: %d"), packet.rooms_size());

    TArray<FRoomListItemInfo> Rooms;

    // Protocol의 room 목록을 순회하면서 FRoomListItemInfo로 변환
    for (const auto& protoRoom : packet.rooms())
    {
        FRoomListItemInfo RoomInfo;
        // Protocol 데이터를 FRoomListItemInfo에 복사
        RoomInfo.RoomId = protoRoom.roomid();
        RoomInfo.RoomName = FString(UTF8_TO_TCHAR(protoRoom.roomname().c_str()));
        RoomInfo.RoomType = static_cast<EGameMode>(protoRoom.roomtype());
        RoomInfo.CurrentPlayers = protoRoom.currentplayers();
        RoomInfo.MaxPlayers = protoRoom.maxplayers();
        RoomInfo.State = static_cast<ERoomState>(protoRoom.state());
        RoomInfo.MapName = FString(UTF8_TO_TCHAR(protoRoom.mapname().c_str()));

        // 각 방의 상세 정보 로깅
        UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] Room Detail:"
            "\n\tRoom ID: %d"
            "\n\tName: %s"
            "\n\tType: %d"
            "\n\tPlayers: %d/%d"
            "\n\tState: %d"
            "\n\tMap: %s"),
            RoomInfo.RoomId,
            *RoomInfo.RoomName,
            static_cast<int32>(RoomInfo.RoomType),
            RoomInfo.CurrentPlayers,
            RoomInfo.MaxPlayers,
            static_cast<int32>(RoomInfo.State),
            *RoomInfo.MapName);

        Rooms.Add(RoomInfo);
    }

    // 변환 완료된 방 배열의 크기 로깅
    UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] Converted room list - Final room count: %d"), Rooms.Num());

    // 게임 화면에도 디버그 메시지 표시 (선택사항)
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
        FString::Printf(TEXT("Received Rooms: %d"), Rooms.Num()));

    // 델리게이트를 통해 변환된 데이터 전달
    OnRoomListResponse.Broadcast(Rooms);
}

void UBlasterNetworkSubsystem::SendCreateRoomReq(const FString& Title, EGameMode GameMode, int32 MaxPlayers)
{
    Protocol::C_CreateRoomReq CreateRoomPacket;
    CreateRoomPacket.set_mapname("temp map");
    CreateRoomPacket.set_title(TCHAR_TO_UTF8(*Title));
    CreateRoomPacket.set_mode(static_cast<Protocol::EGameMode>(GameMode));
    CreateRoomPacket.set_maxplayers(MaxPlayers);

    SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(CreateRoomPacket);
    SendPacket(SendBuffer);
}

void UBlasterNetworkSubsystem::HandleCreateRoomRes(Protocol::S_CreateRoomRes& packet)
{
    UE_LOG(LogTemp, Log, TEXT("[HandleCreateRoomRes] Started processing packet"));

    bool Success = packet.success();
    UE_LOG(LogTemp, Log, TEXT("[HandleCreateRoomRes] Success: %s"), Success ? TEXT("true") : TEXT("false"));

    FRoomDetailInfo RoomInfo;
    if (Success && packet.has_room())
    {
        UE_LOG(LogTemp, Log, TEXT("[HandleCreateRoomRes] Packet has room info"));
        const auto& protoRoom = packet.room();

        // 기본 정보 복사
        RoomInfo.RoomId = protoRoom.roomid();
        RoomInfo.RoomName = UTF8_TO_TCHAR(protoRoom.roomname().c_str());
        RoomInfo.RoomType = static_cast<EGameMode>(protoRoom.roomtype());
        RoomInfo.MaxPlayers = protoRoom.maxplayers();
        RoomInfo.State = static_cast<ERoomState>(protoRoom.state());
        RoomInfo.MapName = UTF8_TO_TCHAR(protoRoom.mapname().c_str());
        RoomInfo.HostPlayerId = protoRoom.hostplayerid();

        UE_LOG(LogTemp, Log, TEXT("[HandleCreateRoomRes] Room Details:"));
        UE_LOG(LogTemp, Log, TEXT("\tRoom ID: %d"), RoomInfo.RoomId);
        UE_LOG(LogTemp, Log, TEXT("\tRoom Name: %s"), *RoomInfo.RoomName);
        UE_LOG(LogTemp, Log, TEXT("\tMax Players: %d"), RoomInfo.MaxPlayers);
        UE_LOG(LogTemp, Log, TEXT("\tHost Player ID: %d"), RoomInfo.HostPlayerId);

        // 플레이어 정보 복사
        for (const auto& protoPlayer : protoRoom.players())
        {
            FPlayerInfo PlayerInfo;
            PlayerInfo.PlayerId = protoPlayer.playerid();
            PlayerInfo.PlayerName = UTF8_TO_TCHAR(protoPlayer.playername().c_str());
            PlayerInfo.IsHost = protoPlayer.ishost();
            PlayerInfo.Team = static_cast<ETeamType>(protoPlayer.team());
            RoomInfo.Players.Add(PlayerInfo);

            UE_LOG(LogTemp, Log, TEXT("\tPlayer: ID=%d, Name=%s, IsHost=%s"),
                PlayerInfo.PlayerId,
                *PlayerInfo.PlayerName,
                PlayerInfo.IsHost ? TEXT("true") : TEXT("false"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[HandleCreateRoomRes] No room info in packet"));
    }

    OnConfirmCreateRoomResponse.Broadcast(Success, RoomInfo);
}

void UBlasterNetworkSubsystem::SendJoinRoomReq(int roomId)
{
    Protocol::C_JoinRoomReq JoinRoomPacket;
    JoinRoomPacket.set_roomid(roomId);
    SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(JoinRoomPacket);
    SendPacket(SendBuffer);
}

void UBlasterNetworkSubsystem::HandleJoinRoomRes(Protocol::S_JoinRoomRes& packet)
{
    UE_LOG(LogTemp, Log, TEXT("[HandleJoinRoomRes] Started processing packet"));

    // 성공 여부 저장
    bool Success = packet.success();
    UE_LOG(LogTemp, Log, TEXT("[HandleJoinRoomRes] Success: %s"), Success ? TEXT("true") : TEXT("false"));

    // RoomDetailInfo 변환
    FRoomDetailInfo RoomInfo;
    if (Success && packet.has_room())
    {
        UE_LOG(LogTemp, Log, TEXT("[HandleJoinRoomRes] Packet has room info"));
        const auto& protoRoom = packet.room();

        // 기본 정보 복사
        RoomInfo.RoomId = protoRoom.roomid();
        RoomInfo.RoomName = UTF8_TO_TCHAR(protoRoom.roomname().c_str());
        RoomInfo.RoomType = static_cast<EGameMode>(protoRoom.roomtype());
        RoomInfo.MaxPlayers = protoRoom.maxplayers();
        RoomInfo.State = static_cast<ERoomState>(protoRoom.state());
        RoomInfo.MapName = UTF8_TO_TCHAR(protoRoom.mapname().c_str());
        RoomInfo.HostPlayerId = protoRoom.hostplayerid();

        UE_LOG(LogTemp, Log, TEXT("[HandleJoinRoomRes] Room Details:"));
        UE_LOG(LogTemp, Log, TEXT("\tRoom ID: %d"), RoomInfo.RoomId);
        UE_LOG(LogTemp, Log, TEXT("\tRoom Name: %s"), *RoomInfo.RoomName);
        UE_LOG(LogTemp, Log, TEXT("\tMax Players: %d"), RoomInfo.MaxPlayers);
        UE_LOG(LogTemp, Log, TEXT("\tHost Player ID: %d"), RoomInfo.HostPlayerId);

        // 플레이어 정보 복사
        for (const auto& protoPlayer : protoRoom.players())
        {
            FPlayerInfo PlayerInfo;
            PlayerInfo.PlayerId = protoPlayer.playerid();
            PlayerInfo.PlayerName = UTF8_TO_TCHAR(protoPlayer.playername().c_str());
            PlayerInfo.IsHost = protoPlayer.ishost();
            PlayerInfo.Team = static_cast<ETeamType>(protoPlayer.team());
            RoomInfo.Players.Add(PlayerInfo);

            UE_LOG(LogTemp, Log, TEXT("\tPlayer: ID=%d, Name=%s, IsHost=%s"),
                PlayerInfo.PlayerId,
                *PlayerInfo.PlayerName,
                PlayerInfo.IsHost ? TEXT("true") : TEXT("false"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[HandleJoinRoomRes] No room info in packet"));
    }

    // 델리게이트 호출 전 바인딩 확인
    bool bIsBound = OnJoinRoomResponse.IsBound();
    UE_LOG(LogTemp, Log, TEXT("[HandleJoinRoomRes] Delegate is bound: %s"), bIsBound ? TEXT("true") : TEXT("false"));

    // 델리게이트를 통해 결과 전달
    UE_LOG(LogTemp, Log, TEXT("[HandleJoinRoomRes] Broadcasting response"));
    OnJoinRoomResponse.Broadcast(Success, RoomInfo);
    UE_LOG(LogTemp, Log, TEXT("[HandleJoinRoomRes] Broadcast completed"));
}

void UBlasterNetworkSubsystem::SendRoomChat(const FString& Message)
{
    Protocol::C_RoomChat chatPacket;
    chatPacket.set_message(TCHAR_TO_UTF8(*Message));

    SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(chatPacket);
    SendPacket(SendBuffer);
}

void UBlasterNetworkSubsystem::HandleBroadcastRoomChat(Protocol::S_BroadcastRoomChat& packet)
{
    int32 PlayerId = packet.playerid();
    FString PlayerName = UTF8_TO_TCHAR(packet.playername().c_str());
    FString Message = UTF8_TO_TCHAR(packet.message().c_str());

    UE_LOG(LogTemp, Log, TEXT("[HandleBroadcastRoomChat] Received message - PlayerID: %d, Name: %s"), PlayerId, *PlayerName);
    UE_LOG(LogTemp, Log, TEXT("[HandleBroadcastRoomChat] Message content: %s"), *Message);

    OnRoomChatMessage.Broadcast(PlayerId, PlayerName, Message);
}

void UBlasterNetworkSubsystem::HandlePing()
{
    UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] Handling Ping"));
    SendPong();
}

void UBlasterNetworkSubsystem::SendPong()
{
    UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] Sending Pong"));
    Protocol::C_Pong PongPacket;
    SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(PongPacket);
    SendPacket(SendBuffer);
}
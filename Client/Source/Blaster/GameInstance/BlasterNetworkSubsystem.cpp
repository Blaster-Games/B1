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
    if (GameServerSession)
    {
        GameServerSession->Stop();  // PacketSession에 Stop 함수 추가 필요
    }

    DisconnectFromGameServer();
    Super::Deinitialize();
}

void UBlasterNetworkSubsystem::ConnectToGameServer()
{
    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));
    FIPv4Address Ip;
    FIPv4Address::Parse(IpAddress, Ip);
    TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    InternetAddr->SetIp(Ip.Value);

    // 포트 설정을 더 명확하게
    uint16 SafePort = static_cast<uint16>(Port);  // 명시적으로 uint16으로 변환
    InternetAddr->SetPort(SafePort);

    UE_LOG(LogTemp, Warning, TEXT("Attempting connection to %s:%u"), *IpAddress, SafePort);  // %u로 변경

    bool Connected = Socket->Connect(*InternetAddr);
    if (Connected)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));
        GameServerSession = MakeShared<PacketSession>(Socket);
        GameServerSession->Run();
    }
    else
    {
        ESocketErrors LastError = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();
        UE_LOG(LogTemp, Error, TEXT("Connection Failed with error: %d"), (int32)LastError);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
    }
}

void UBlasterNetworkSubsystem::LoadNetworkSettings()
{
    if (UBlasterGameInstance* GameInstance = Cast<UBlasterGameInstance>(GetGameInstance()))
    {
        IpAddress = GameInstance->GetDefaultServerIP();
        Port = GameInstance->GetDefaultServerPort();
    }
}

//void UBlasterNetworkSubsystem::ConnectToGameServer()
//{
//    if (!IsInGameThread())
//    {
//        AsyncTask(ENamedThreads::GameThread, [this]()
//            {
//                ConnectToGameServer();
//            });
//        return;
//    }
//
//    // 이미 연결되어 있다면 연결 해제
//    if (Socket)
//    {
//        DisconnectFromGameServer();
//    }
//
//    // 소켓 생성 전 서브시스템 체크
//    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
//    if (!SocketSubsystem)
//    {
//        return;
//    }
//
//    Socket = SocketSubsystem->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));
//    if (!Socket)
//    {
//        return;
//    }
//
//    // 소켓 옵션 설정
//    Socket->SetNonBlocking(true);
//    Socket->SetReuseAddr(true);
//
//    // 타임아웃 설정
//    int32 ActualSize;  // 여기에 변수 선언
//    Socket->SetSendBufferSize(64 * 1024, ActualSize);
//    Socket->SetReceiveBufferSize(64 * 1024, ActualSize);
//
//    // 접속 시도 전 로그
//    UE_LOG(LogTemp, Log, TEXT("Attempting to connect to %s:%d"), *IpAddress, Port);
//
//    FIPv4Address Ip;
//    FIPv4Address::Parse(IpAddress, Ip);
//    TSharedPtr<FInternetAddr> InternetAddr = SocketSubsystem->CreateInternetAddr();
//    InternetAddr->SetIp(Ip.Value);
//    InternetAddr->SetPort(Port);
//
//    // 연결 시도
//    bool Connected = Socket->Connect(*InternetAddr);
//
//    if (Connected)
//    {
//        UE_LOG(LogTemp, Log, TEXT("Connection Success"));
//        GameServerSession = MakeShared<PacketSession>(Socket);
//        GameServerSession->Run();
//    }
//    else
//    {
//        UE_LOG(LogTemp, Warning, TEXT("Connection Failed"));
//        DisconnectFromGameServer();
//    }
//}

void UBlasterNetworkSubsystem::DisconnectFromGameServer()
{
    UE_LOG(LogTemp, Warning, TEXT("Disconnected from server"));

    if (GameServerSession)
    {
        GameServerSession->Stop();
        GameServerSession.Reset();
    }

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
        AuthReq.set_jwt(TCHAR_TO_UTF8(*GameInst->GetAccessToken()));
        AuthReq.set_accountdbid(GameInst->GetUserId());
        AuthReq.set_nickname(TCHAR_TO_UTF8(*GameInst->GetNickname()));

        // Protocol Buffer 객체 검증
        UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] Protocol Buffer - Nickname set: %s"),
            UTF8_TO_TCHAR(AuthReq.nickname().c_str()));
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
        if (UBlasterGameInstance* GameInst = Cast<UBlasterGameInstance>(GetGameInstance()))
        {
            GameInst->SetPlayerId(packet.playerid());
            UE_LOG(LogTemp, Log, TEXT("[HandleAuthRes] Current PlayerId is : %d"), GameInst->GetPlayerId());
        }
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
        RoomInfo.MapName = FString(UTF8_TO_TCHAR("Highrise"));

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
    CreateRoomPacket.set_mapname("Highrise");
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
        // TODO 맵이름 바꿔야됨
        RoomInfo.MapName = UTF8_TO_TCHAR("HighRise");
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
        RoomInfo.MapName = UTF8_TO_TCHAR("Highrise");
        RoomInfo.HostPlayerId = protoRoom.hostplayerid();

        UE_LOG(LogTemp, Log, TEXT("[HandleJoinRoomRes] Room Details:"));
        UE_LOG(LogTemp, Log, TEXT("\tRoom ID: %d"), RoomInfo.RoomId);
        UE_LOG(LogTemp, Log, TEXT("\tRoom Name: %s"), *RoomInfo.RoomName);
        UE_LOG(LogTemp, Log, TEXT("\tMax Players: %d"), RoomInfo.MaxPlayers);
        UE_LOG(LogTemp, Log, TEXT("\tHost Player ID: %d"), RoomInfo.HostPlayerId);

        // 플레이어 정보 복사
        for (const auto& player : protoRoom.players())
        {
            FPlayerInfo PlayerInfo;
            PlayerInfo.PlayerId = player.playerid();
            PlayerInfo.PlayerName = UTF8_TO_TCHAR(player.playername().c_str());
            PlayerInfo.IsHost = player.ishost();
            PlayerInfo.Team = static_cast<ETeamType>(player.team());
            PlayerInfo.SlotNumber = player.slotnumber();
            RoomInfo.Players.Add(PlayerInfo);

            UE_LOG(LogTemp, Log, TEXT("Received from server - PlayerName: %s, SlotNumber raw: %d"),
                *FString(UTF8_TO_TCHAR(player.playername().c_str())),
                player.slotnumber());

            UE_LOG(LogTemp, Log, TEXT("\tPlayer: ID=%d, Name=%s, IsHost=%s, SlotNumber=%d"),
                player.playerid(),
                *FString(UTF8_TO_TCHAR(player.playername().c_str())),
                player.ishost() ? TEXT("true") : TEXT("false"),
                player.slotnumber()
            );
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

void UBlasterNetworkSubsystem::HandleBroadcastJoinRoom(Protocol::S_BroadcastJoinRoom& packet)
{
    FRoomDetailInfo RoomInfo;

    // packet.room()을 통해 RoomDetailInfo에 접근
    RoomInfo.RoomId = packet.room().roomid();
    RoomInfo.RoomName = FString(UTF8_TO_TCHAR(packet.room().roomname().c_str()));
    RoomInfo.RoomType = static_cast<EGameMode>(packet.room().roomtype());
    RoomInfo.MaxPlayers = packet.room().maxplayers();
    RoomInfo.State = static_cast<ERoomState>(packet.room().state());
    RoomInfo.MapName = FString(UTF8_TO_TCHAR("Highrise"));
    RoomInfo.HostPlayerId = packet.room().hostplayerid();

    // Players 배열 변환
    RoomInfo.Players.Empty();
    for (const auto& player : packet.room().players())
    {
        FPlayerInfo playerInfo;
        playerInfo.PlayerId = player.playerid();
        playerInfo.PlayerName = FString(UTF8_TO_TCHAR(player.playername().c_str()));
        playerInfo.IsHost = player.ishost();
        playerInfo.Team = static_cast<ETeamType>(player.team());
		playerInfo.SlotNumber = player.slotnumber();
        RoomInfo.Players.Add(playerInfo);

        UE_LOG(LogTemp, Log, TEXT("Received from server - PlayerName: %s, SlotNumber raw: %d"),
            *FString(UTF8_TO_TCHAR(player.playername().c_str())),
            player.slotnumber());

        // SlotNumber 로깅
        UE_LOG(LogTemp, Log, TEXT("\tPlayer: ID=%d, Name=%s, IsHost=%s, SlotNumber=%d"),
            player.playerid(),
            *FString(UTF8_TO_TCHAR(player.playername().c_str())),
            player.ishost() ? TEXT("true") : TEXT("false"),
            player.slotnumber()
        );
    }
	OnBroadcastJoinRoom.Broadcast(RoomInfo);
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

void UBlasterNetworkSubsystem::SendStartGameReq(const FString& HostAddress, int32 ServerPort)
{
    Protocol::C_StartGameReq StartGamePacket;
    StartGamePacket.set_hostaddress(TCHAR_TO_UTF8(*HostAddress));
    StartGamePacket.set_port(ServerPort);
    SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(StartGamePacket);
    SendPacket(SendBuffer);
}

void UBlasterNetworkSubsystem::HandleStartGameRes(Protocol::S_StartGameRes& packet)
{
}

void UBlasterNetworkSubsystem::HandleBroadcastStartGame(Protocol::S_BroadcastStartGame& packet)
{
	FString HostAddress = UTF8_TO_TCHAR(packet.hostaddress().c_str());
	int32 HostPort = packet.port();

	OnBroadcastStartGame.Broadcast(HostAddress, HostPort);
}

void UBlasterNetworkSubsystem::SendLeaveRoomReq()
{
	UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] Sending LeaveRoomReq"));
	Protocol::C_LeaveRoomReq LeaveRoomPacket;
	SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(LeaveRoomPacket);
	SendPacket(SendBuffer);
}

void UBlasterNetworkSubsystem::HandleLeaveRoomRes(Protocol::S_LeaveRoomRes& packet)
{
    UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] Handling LeaveRoomRes - Success: %s"), packet.success() ? TEXT("True") : TEXT("False"));
    OnLeaveRoomResponse.Broadcast(packet.success());
    UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] LeaveRoomRes broadcast completed"));
}

void UBlasterNetworkSubsystem::HandleBroadcastLeaveRoom(Protocol::S_BroadcastLeaveRoom& packet)
{
    // Protocol::RoomDetailInfo를 FRoomDetailInfo로 변환
    FRoomDetailInfo roomInfo;
    roomInfo.RoomId = packet.room().roomid();
    roomInfo.RoomName = FString(packet.room().roomname().c_str());
    roomInfo.RoomType = static_cast<EGameMode>(packet.room().roomtype());
    roomInfo.MaxPlayers = packet.room().maxplayers();
    roomInfo.State = static_cast<ERoomState>(packet.room().state());
    roomInfo.MapName = FString(UTF8_TO_TCHAR("Highrise"));
    roomInfo.HostPlayerId = packet.room().hostplayerid();

    // 플레이어 정보 변환
    for (const auto& protoPlayer : packet.room().players())
    {
        FPlayerInfo playerInfo;
        playerInfo.PlayerId = protoPlayer.playerid();
        playerInfo.PlayerName = FString(protoPlayer.playername().c_str());
        playerInfo.IsHost = protoPlayer.ishost();
        playerInfo.Team = static_cast<ETeamType>(protoPlayer.team());
        playerInfo.SlotNumber = protoPlayer.slotnumber();
        roomInfo.Players.Add(playerInfo);
    }

    OnBroadcastLeaveRoom.Broadcast(roomInfo);
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
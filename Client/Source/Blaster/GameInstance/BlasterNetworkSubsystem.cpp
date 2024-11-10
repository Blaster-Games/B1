#include "BlasterNetworkSubsystem.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "BlasterGameInstance.h"

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

        // 로그 출력
        UE_LOG(LogTemp, Log, TEXT("[NetworkSubsystem] SendAuthReq - JWT: %s, AccountDbId: %d"),
            *GameInst->AccessToken, GameInst->UserId);

        // 화면에도 디버그 메시지 표시 (선택사항)
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
            FString::Printf(TEXT("Sending Auth - ID: %d"), GameInst->UserId));
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
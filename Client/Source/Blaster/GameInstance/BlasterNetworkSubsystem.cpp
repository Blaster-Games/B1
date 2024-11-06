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
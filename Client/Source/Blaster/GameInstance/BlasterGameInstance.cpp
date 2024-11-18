#include "BlasterGameInstance.h"
#include "GameInstance/BlasterNetworkSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "IPAddress.h"
#include "Sockets.h"

void UBlasterGameInstance::Init()
{
    Super::Init();

    NetworkSystem = GetSubsystem<UBlasterNetworkSubsystem>();
}

void UBlasterGameInstance::ConnectToServer()
{
    if (NetworkSystem)
    {
        NetworkSystem->ConnectToGameServer();
    }
}

void UBlasterGameInstance::DisconnectFromServer()
{
    if (NetworkSystem)
    {
        NetworkSystem->DisconnectFromGameServer();
    }
}

void UBlasterGameInstance::HandleNetworkPackets()
{
    if (NetworkSystem)
    {
        NetworkSystem->HandleRecvPackets();
    }
}

void UBlasterGameInstance::SendNetworkPacket(SendBufferRef SendBuffer)
{
    if (NetworkSystem)
    {
        NetworkSystem->SendPacket(SendBuffer);
    }
}

void UBlasterGameInstance::HostGame()
{
    const int32 ServerPort = 7777;

    // IP 주소 가져오기
    bool bCanBindAll;
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(NAME_None);
    FString HostAddress;
    if (SocketSubsystem)
    {
        TSharedPtr<FInternetAddr> LocalIP = SocketSubsystem->GetLocalHostAddr(*GLog, bCanBindAll);
        if (LocalIP.IsValid())
        {
            HostAddress = LocalIP->ToString(false);
        }
    }

    UWorld* World = GetWorld();
    if (!ensure(World != nullptr)) return;

    // 현재 월드를 리슨서버로 변환
    FURL ListenURL;
    ListenURL.Port = ServerPort;

    // 리슨서버 시작
    if (World->Listen(ListenURL))
    {
        UE_LOG(LogTemp, Warning, TEXT("Listen Server started successfully on port: %d"), ServerPort);

        // 리슨서버 시작 성공 시 게임 서버에 알림
        if (UBlasterNetworkSubsystem* NetSystem = GetSubsystem<UBlasterNetworkSubsystem>())
        {
            NetSystem->SendStartGameReq(HostAddress, ServerPort);
            UE_LOG(LogTemp, Warning, TEXT("Sending info to game server: %s:%d"), *HostAddress, ServerPort);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to start Listen Server"));
    }
}

// 게임에 참가하는 코드 (다른 플레이어용)
void UBlasterGameInstance::JoinGame(const FString& Address, int32 Port)
{
    UWorld* World = GetWorld();
    if (!ensure(World != nullptr)) return;

    // 서버 주소 구성
    FString ServerAddress = FString::Printf(TEXT("%s:%d"), *Address, Port);

    // 서버에 접속
    APlayerController* PC = World->GetFirstPlayerController();
    if (PC)
    {
        PC->ClientTravel(ServerAddress, TRAVEL_Absolute);
    }
}

void UBlasterGameInstance::UpdateCurrentRoomInfo(const FRoomDetailInfo& NewRoomInfo)
{
    CurrentRoomInfo = NewRoomInfo;
}

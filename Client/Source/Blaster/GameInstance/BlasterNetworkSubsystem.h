#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PacketSession.h"
#include "BlasterNetworkSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthSuccessDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuthFailedDelegate, const FString&, ErrorMessage);

UCLASS()
class BLASTER_API UBlasterNetworkSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    friend class UBlasterGameInstance;

public:
    UPROPERTY(BlueprintAssignable, Category = "Network|Auth")
    FOnAuthSuccessDelegate OnAuthSuccess;

    UPROPERTY(BlueprintAssignable, Category = "Network|Auth")
    FOnAuthFailedDelegate OnAuthFailed;

public:
    void HandleAuthRes(Protocol::S_AuthRes& packet);
    void SendAuthReq();
    void HandlePing();
    void SendPong();

public:
    class FSocket* Socket;
    TSharedPtr<class PacketSession> GameServerSession;

    // 서버 연결 정보
    FString IpAddress = TEXT("127.0.0.1");
    int16 Port = 7777;

private:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    void ConnectToGameServer();
    void DisconnectFromGameServer();
    void HandleRecvPackets();
    void SendPacket(SendBufferRef SendBuffer);

    // 네트워크 설정 로드
    void LoadNetworkSettings();
};
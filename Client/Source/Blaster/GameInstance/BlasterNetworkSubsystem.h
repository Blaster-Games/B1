#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PacketSession.h"
#include "BlasterNetworkSubsystem.generated.h"

/**
 * 네트워크 통신을 담당하는 서브시스템
 */
UCLASS()
class BLASTER_API UBlasterNetworkSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    // GameInstance만 접근 가능하도록 friend 선언
    friend class UBlasterGameInstance;

private:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    void ConnectToGameServer();
    void DisconnectFromGameServer();
    void HandleRecvPackets();
    void SendPacket(SendBufferRef SendBuffer);

    // 네트워크 설정 로드
    void LoadNetworkSettings();

private:
    class FSocket* Socket;
    TSharedPtr<class PacketSession> GameServerSession;

    // 서버 연결 정보
    FString IpAddress = TEXT("127.0.0.1");
    int16 Port = 7777;
};
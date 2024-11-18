#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blaster.h"
#include "HUD/Lobby/RoomTypes.h"
#include "BlasterGameInstance.generated.h"

UCLASS()
class BLASTER_API UBlasterGameInstance : public UGameInstance, public FTickableGameObject
{
    GENERATED_BODY()
public:
    virtual void Init() override;

    virtual void Tick(float DeltaTime) override
    {
        HandleNetworkPackets();
    }
    virtual bool IsTickable() const override { return true; }
    virtual TStatId GetStatId() const override { return TStatId(); }

    UFUNCTION(BlueprintCallable)
    void ConnectToServer();

    UFUNCTION(BlueprintCallable)
    void DisconnectFromServer();

    UFUNCTION(BlueprintCallable)
    void HandleNetworkPackets();

    void SendNetworkPacket(SendBufferRef SendBuffer);

    UFUNCTION(BlueprintCallable, Category = "Network")
    void HostGame();

    UFUNCTION(BlueprintCallable, Category = "Network")
    void JoinGame(const FString& Address, int32 Port);

    // Getter
    const FRoomDetailInfo& GetCurrentRoomInfo() const { return CurrentRoomInfo; }

    // Setter with broadcast
    UFUNCTION()
    void UpdateCurrentRoomInfo(const FRoomDetailInfo& NewRoomInfo);

    UPROPERTY(Config)
    FString DefaultServerIP = TEXT("127.0.0.1");

    UPROPERTY(Config)
    int16 DefaultServerPort = 7777;

    // 인증 및 사용자 데이터
    FString AccessToken;
    FString RefreshToken;
    FString Nickname;
    int32 UserId;

private:
    UPROPERTY()
    class UBlasterNetworkSubsystem* NetworkSystem;

    FRoomDetailInfo CurrentRoomInfo;
};
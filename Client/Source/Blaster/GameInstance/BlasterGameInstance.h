#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blaster.h"
#include "BlasterGameInstance.generated.h"

UCLASS()
class BLASTER_API UBlasterGameInstance : public UGameInstance
{
    GENERATED_BODY()
public:
    virtual void Init() override;

    UFUNCTION(BlueprintCallable)
    void ConnectToServer();

    UFUNCTION(BlueprintCallable)
    void DisconnectFromServer();

    UFUNCTION(BlueprintCallable)
    void HandleNetworkPackets();

    void SendNetworkPacket(SendBufferRef SendBuffer);

    UPROPERTY(Config)
    FString DefaultServerIP = TEXT("127.0.0.1");

    UPROPERTY(Config)
    int16 DefaultServerPort = 7777;

private:
    UPROPERTY()
    class UBlasterNetworkSubsystem* NetworkSystem;
};
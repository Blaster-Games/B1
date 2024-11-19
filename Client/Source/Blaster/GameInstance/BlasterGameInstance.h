#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blaster.h"
#include "Blaster/Save/BlasterSaveSettings.h"
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

    UPROPERTY(Config)
    FString DefaultServerIP = TEXT("127.0.0.1");

    UPROPERTY(Config)
    int16 DefaultServerPort = 7777;

    // 인증 및 사용자 데이터
    FString AccessToken;
    FString RefreshToken;
    FString Nickname;
    int32 UserId;

    /**
    * Settings - 아니면 settings 관련 로직을 따로 별도의 서브시스템으로 분리를 해도 될 듯
    */



    UFUNCTION(BlueprintCallable)
    const FSensitivitySettings& GetSensitivitySettings() const { return SensitivitySettings; }

    UFUNCTION(BlueprintCallable)
    void SetSensitivitySettings(const FSensitivitySettings& NewSettings);

    UFUNCTION(BlueprintCallable)
    void SaveSettings();

    UFUNCTION(BlueprintCallable)
    void LoadSettings();


private:
    UPROPERTY()
    class UBlasterNetworkSubsystem* NetworkSystem;

    FSensitivitySettings SensitivitySettings;
};
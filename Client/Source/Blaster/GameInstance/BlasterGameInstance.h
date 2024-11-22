#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blaster.h"
#include "HUD/Lobby/RoomTypes.h"
#include "Blaster/Save/BlasterSaveSettings.h"
#include "BlasterGameInstance.generated.h"

UCLASS()
class BLASTER_API UBlasterGameInstance : public UGameInstance, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Tick(float DeltaTime) override { HandleNetworkPackets(); }
	virtual bool IsTickable() const override { return true; }
	virtual TStatId GetStatId() const override { return TStatId(); }

	// Network Functions
	UFUNCTION(BlueprintCallable)
	void ConnectToServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromServer();

	UFUNCTION(BlueprintCallable)
	void HandleNetworkPackets();

	void SendNetworkPacket(SendBufferRef SendBuffer);

	// Game Session Functions
	UFUNCTION(BlueprintCallable, Category = "Network")
	void HostGame();

	UFUNCTION(BlueprintCallable, Category = "Network")
	void JoinGame(const FString& Address, int32 Port);

	// Getters
	UFUNCTION(BlueprintCallable, Category = "Room")
	const FRoomDetailInfo& GetCurrentRoomInfo() const { return CurrentRoomInfo; }

	UFUNCTION(BlueprintCallable, Category = "User")
	FString GetAccessToken() const { return AccessToken; }

	UFUNCTION(BlueprintCallable, Category = "User")
	FString GetRefreshToken() const { return RefreshToken; }

	UFUNCTION(BlueprintCallable, Category = "User")
	FString GetNickname() const { return Nickname; }

	UFUNCTION(BlueprintCallable, Category = "User")
	int32 GetUserId() const { return UserId; }

	UFUNCTION(BlueprintCallable, Category = "User")
	int32 GetPlayerId() const { return PlayerId; }

	UFUNCTION(BlueprintCallable, Category = "User")
	bool GetIsLogin() const { return IsLogin; }

	UFUNCTION(BlueprintCallable, Category = "User")
	bool GetShouldShowRoom() const { return ShouldShowRoom; }

	// Network Configuration Getters
	FString GetDefaultServerIP() const { return DefaultServerIP; }
	int16 GetDefaultServerPort() const { return DefaultServerPort; }

	// Setters
	UFUNCTION(BlueprintCallable, Category = "Room")
	void UpdateCurrentRoomInfo(const FRoomDetailInfo& NewRoomInfo);

	UFUNCTION(BlueprintCallable, Category = "User")
	void SetAccessToken(const FString& NewToken) { AccessToken = NewToken; }

	UFUNCTION(BlueprintCallable, Category = "User")
	void SetRefreshToken(const FString& NewToken) { RefreshToken = NewToken; }

	UFUNCTION(BlueprintCallable, Category = "User")
	void SetNickname(const FString& NewNickname) { Nickname = NewNickname; }

	UFUNCTION(BlueprintCallable, Category = "User")
	void SetUserId(int32 NewUserId) { UserId = NewUserId; }

	UFUNCTION(BlueprintCallable, Category = "User")
	void SetPlayerId(int32 NewPlayerId) { PlayerId = NewPlayerId; }

	UFUNCTION(BlueprintCallable, Category = "User")
	void SetIsLogin(bool NewIsLogin) { IsLogin = NewIsLogin; }

	UFUNCTION()
	void SetShowRoomFlag(bool bFlag) { ShouldShowRoom = bFlag; }

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
	//UPROPERTY(Config)
	//FString DefaultServerIP = TEXT("34.64.137.161");

	//UPROPERTY(Config)
	//int32 DefaultServerPort = 7777;

	UPROPERTY(Config)
	FString DefaultServerIP = TEXT("127.0.0.1");

	UPROPERTY(Config)
	int32 DefaultServerPort = 7777;

	UPROPERTY()
	class UBlasterNetworkSubsystem* NetworkSystem;

	FRoomDetailInfo CurrentRoomInfo;

	// User Data
	FString AccessToken;
	FString RefreshToken;
	FString Nickname;
	int32 UserId;
	int32 PlayerId;
	bool IsLogin;
	bool ShouldShowRoom = false;

	FSensitivitySettings SensitivitySettings;
};
#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PacketSession.h"
#include "BlasterNetworkSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthSuccessDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuthFailedDelegate, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterLobbyResponseDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomListResponseDelegate, const TArray<FRoomListItemInfo>&, Rooms);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJoinRoomResponseDelegate, bool, Success, const FRoomDetailInfo&, RoomInfo);

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

    UPROPERTY(BlueprintAssignable, Category = "Network|Lobby")
    FOnEnterLobbyResponseDelegate OnEnterLobbyResponse;

    UPROPERTY(BlueprintAssignable, Category = "Network|Lobby")
    FOnRoomListResponseDelegate OnRoomListResponse;

    UPROPERTY(BlueprintAssignable, Category = "Network|Room")
    FOnJoinRoomResponseDelegate OnJoinRoomResponse;

public:
    void SendAuthReq();
    void HandleAuthRes(Protocol::S_AuthRes& packet);

    void SendEnterLobbyReq();
    void HandleEnterLobbyRes(Protocol::S_EnterLobbyRes& packet);

    void SendRoomListReq();
    void HandleRoomListRes(Protocol::S_RoomListRes& packet);

    void SendJoinRoomReq(int roomId);
    void HandleJoinRoomRes(Protocol::S_JoinRoomRes& packet);

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
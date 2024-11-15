#include "Lobby.h"
#include "GameInstance/BlasterNetworkSubsystem.h"
#include "RoomList.h"

void ULobby::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Log, TEXT("Lobby NativeConstruct Start"));

    if (RoomListWidget)
    {
        // RoomItem의 더블클릭 이벤트 바인딩
        RoomListWidget->OnRoomSelected.AddDynamic(this, &ULobby::HandleRoomItemDoubleClicked);
        UE_LOG(LogTemp, Log, TEXT("Lobby: Successfully bound RoomList's OnRoomSelected to HandleRoomItemDoubleClicked"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Lobby: RoomListWidget is null"));
    }

    // NetworkSubsystem의 JoinRoom 응답 이벤트 바인딩
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            NetworkSubsystem->OnJoinRoomResponse.AddDynamic(this, &ULobby::HandleJoinRoomResponse);
            UE_LOG(LogTemp, Log, TEXT("Lobby: Successfully bound NetworkSubsystem's OnJoinRoomResponse"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Lobby: Failed to get NetworkSubsystem"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Lobby: Failed to get GameInstance"));
    }

    UE_LOG(LogTemp, Log, TEXT("Lobby NativeConstruct End"));
}

void ULobby::HandleRoomItemDoubleClicked(int32 RoomId)
{
    UE_LOG(LogTemp, Log, TEXT("Lobby: HandleRoomItemDoubleClicked called with RoomId: %d"), RoomId);

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            UE_LOG(LogTemp, Log, TEXT("Lobby: Sending JoinRoomReq for RoomId: %d"), RoomId);
            NetworkSubsystem->SendJoinRoomReq(RoomId);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Lobby: Failed to get NetworkSubsystem in HandleRoomItemDoubleClicked"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Lobby: Failed to get GameInstance in HandleRoomItemDoubleClicked"));
    }
}

void ULobby::HandleJoinRoomResponse(bool Success, const FRoomDetailInfo& RoomInfo)
{
    UE_LOG(LogTemp, Log, TEXT("Lobby: HandleJoinRoomResponse called - Success: %s"), Success ? TEXT("true") : TEXT("false"));

    if (Success)
    {
        UE_LOG(LogTemp, Log, TEXT("Lobby: Join room successful, showing Room widget"));
        ShowRoomWidget(RoomInfo);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Lobby: Failed to join room"));
        // 여기에 실패 UI 표시 로직 추가 가능
    }
}

void ULobby::ShowRoomWidget(const FRoomDetailInfo& RoomInfo)
{
    UE_LOG(LogTemp, Log, TEXT("Lobby: ShowRoomWidget called"));
    RemoveFromParent();

    if (RoomWidgetClass)
    {
        if (URoomDetail* RoomWidget = CreateWidget<URoomDetail>(GetWorld(), RoomWidgetClass))
        {
            RoomWidget->UpdateRoomInfo(RoomInfo);

            UE_LOG(LogTemp, Log, TEXT("Lobby: Created RoomWidget and updated room info"));
            RoomWidget->AddToViewport();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to Create RoomWidget"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("RoomWidgetClass is not set!"));
    }
}
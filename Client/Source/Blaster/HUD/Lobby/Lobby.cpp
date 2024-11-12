#include "Lobby.h"
#include "GameInstance/BlasterNetworkSubsystem.h"
#include "RoomList.h"

void ULobby::NativeConstruct()
{
    Super::NativeConstruct();

    if (RoomListWidget)
    {
        // RoomItem의 더블클릭 이벤트 바인딩
        RoomListWidget->OnRoomSelected.AddDynamic(this, &ULobby::HandleRoomItemDoubleClicked);
    }

    // NetworkSubsystem의 JoinRoom 응답 이벤트 바인딩
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem =
            GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            NetworkSubsystem->OnJoinRoomResponse.AddDynamic(
                this, &ULobby::HandleJoinRoomResponse);
        }
    }
}

void ULobby::HandleRoomItemDoubleClicked(int32 RoomId)
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem =
            GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            NetworkSubsystem->SendJoinRoomReq(RoomId);
        }
    }
}

void ULobby::HandleJoinRoomResponse(bool Success, const FRoomDetailInfo& RoomInfo)
{
    if (Success)
    {
        ShowRoomWidget(RoomInfo);
    }
    else
    {
        // 실패 처리 (예: 메시지 표시)
        UE_LOG(LogTemp, Warning, TEXT("Failed to join room"));
    }
}

void ULobby::ShowRoomWidget(const FRoomDetailInfo& RoomInfo)
{
    if (!RoomWidgetClass) return;

    if (URoom* RoomWidget = CreateWidget<URoom>(this, RoomWidgetClass))
    {
        RemoveFromParent();
        RoomWidget->AddToViewport();

        // Room 정보 설정 (Room 클래스에 해당 메서드 추가 필요)
        RoomWidget->UpdateRoom(RoomInfo);
    }
}
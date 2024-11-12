#include "RoomList.h"
#include "GameInstance/BlasterNetworkSubsystem.h"

void URoomList::NativeConstruct()
{
    Super::NativeConstruct();

    // 버튼 클릭 이벤트 바인딩
    if (CreateRoomButton)
    {
        CreateRoomButton->OnClicked.AddDynamic(this, &URoomList::OnCreateRoomClicked);
    }

    if (UpdateRoomItemButton)
    {
        UpdateRoomItemButton->OnClicked.AddDynamic(this, &URoomList::OnUpdateRoomItemClicked);
    }

    // NetworkSubsystem 이벤트 바인딩
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            NetworkSubsystem->OnRoomListResponse.AddDynamic(this, &URoomList::HandleRoomListResponse);
        }
    }
}

void URoomList::OnCreateRoomClicked()
{
    // TODO: 방 생성 팝업 창 띄우기
}

void URoomList::OnUpdateRoomItemClicked()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            NetworkSubsystem->SendRoomListReq();
        }
    }
}

void URoomList::HandleRoomListResponse(const TArray<FRoomListItemInfo>& Rooms)
{
    UpdateRoomList(Rooms);
}

void URoomList::HandleRoomItemDoubleClicked(int32 RoomId)
{
    OnRoomSelected.Broadcast(RoomId);
}

void URoomList::UpdateRoomList(const TArray<FRoomListItemInfo>& Rooms)
{
    ClearRoomList();
    for (const FRoomListItemInfo& Room : Rooms)
    {
        AddRoom(Room);
    }
}

URoomItem* URoomList::AddRoom(const FRoomListItemInfo& RoomInfo)
{
    if (!RoomListScrollBox || !RoomItemClass)
    {
        return nullptr;
    }

    URoomItem* NewRoomItem = CreateWidget<URoomItem>(this, RoomItemClass);
    if (NewRoomItem)
    {
        NewRoomItem->SetRoomInfo(
            RoomInfo.RoomId,
            RoomInfo.RoomName,
            RoomInfo.RoomType,
            RoomInfo.CurrentPlayers,
            RoomInfo.MaxPlayers,
            RoomInfo.State,
            RoomInfo.MapName
        );

        // 더블클릭 이벤트 바인딩
        NewRoomItem->OnRoomItemClicked.AddDynamic(this, &URoomList::HandleRoomItemDoubleClicked);

        RoomListScrollBox->AddChild(NewRoomItem);
        RoomItems.Add(NewRoomItem);
    }
    return NewRoomItem;
}

void URoomList::ClearRoomList()
{
    if (!RoomListScrollBox) return;

    RoomListScrollBox->ClearChildren();
    RoomItems.Empty();
}
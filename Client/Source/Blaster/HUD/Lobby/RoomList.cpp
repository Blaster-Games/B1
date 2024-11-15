#include "RoomList.h"
#include "GameInstance/BlasterNetworkSubsystem.h"
#include "CreateRoom.h"
#include "Room/RoomDetail.h"

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
    UCreateRoom* NewCreateRoomWidget = CreateWidget<UCreateRoom>(GetWorld(), CreateRoomWidgetClass);
    if (NewCreateRoomWidget)
    {
        NewCreateRoomWidget->AddToViewport(100);
        if (NewCreateRoomWidget)
        {
            NewCreateRoomWidget->OnRoomCreated.AddDynamic(this, &URoomList::HandleCreateRoomResponse);
        }
    }
}

void URoomList::OnUpdateRoomItemClicked()
{
    RequestUpdateRoomList();
}

void URoomList::RequestUpdateRoomList()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            NetworkSubsystem->SendRoomListReq();
        }
    }
}

void URoomList::HandleCreateRoomResponse(const FRoomDetailInfo& RoomInfo)
{
    UE_LOG(LogTemp, Log, TEXT("Lobby: ShowRoomWidget called"));
    RemoveFromParent();

    if (RoomDetailWidgetClass)
    {
        if (URoomDetail* RoomWidget = CreateWidget<URoomDetail>(GetWorld(), RoomDetailWidgetClass))
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
        UE_LOG(LogTemp, Warning, TEXT("AddRoom Failed: RoomListScrollBox or RoomItemClass is null"));
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

        UE_LOG(LogTemp, Log, TEXT("Room ID: %d successfully added to scroll box"), RoomInfo.RoomId);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create RoomItem widget"));
    }

    return NewRoomItem;
}

void URoomList::ClearRoomList()
{
    if (!RoomListScrollBox) return;
    RoomListScrollBox->ClearChildren();
    RoomItems.Empty();
}
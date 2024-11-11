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
    // TODO: 방 리스트 불러와서 RoomItems 새로고침하고 UpdateRoomList 호출하기
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            // 연결 상태 확인 (Socket이나 Session이 유효한지)
            if (NetworkSubsystem->Socket && NetworkSubsystem->GameServerSession)
            {
                NetworkSubsystem->SendRoomListReq();
                UE_LOG(LogTemp, Log, TEXT("[EnterMainMenu] Sending auth request"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("[EnterMainMenu] Not connected to server"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[EnterMainMenu] Failed to get NetworkSubsystem"));
        }
    }
}

void URoomList::HandleRoomListResponse(const TArray<FRoomListItemInfo>& Rooms)
{
    UpdateRoomList(Rooms);
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
    if (!RoomListScrollBox) return nullptr;
    // RoomItem 위젯 생성
    URoomItem* NewRoomItem = CreateWidget<URoomItem>(this, URoomItem::StaticClass());
    if (NewRoomItem)
    {
        // Room 정보 설정
        NewRoomItem->SetRoomInfo(
            RoomInfo.RoomId,
            RoomInfo.RoomName,
            RoomInfo.RoomType,
            RoomInfo.CurrentPlayers,
            RoomInfo.MaxPlayers,
            RoomInfo.State,
            RoomInfo.MapName
        );

        // 스크롤 박스에 추가
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
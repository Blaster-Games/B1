#include "RoomItem.h"
#include "Components/TextBlock.h"

void URoomItem::NativeConstruct()
{
    Super::NativeConstruct();

    if (RoomItemButton)
    {
        RoomItemButton->OnClicked.AddDynamic(this, &URoomItem::OnRoomItemButtonClicked);
    }
}

void URoomItem::OnRoomItemButtonClicked()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float TimeSinceLastClick = CurrentTime - LastClickTime;

    if (TimeSinceLastClick <= DoubleClickTime)
    {
        OnRoomItemClicked.Broadcast(RoomId);
        LastClickTime = 0.0f;
    }
    else
    {
        LastClickTime = CurrentTime;
    }
}

void URoomItem::SetRoomInfo(
    int32 InRoomId,
    const FString& InRoomName,
    EGameMode InRoomType,
    int32 InCurrentPlayers,
    int32 InMaxPlayers,
    ERoomState InState,
    const FString& InMapName
)
{
    RoomId = InRoomId;

    if (RoomIdText)
    {
        RoomIdText->SetText(FText::AsNumber(InRoomId));
    }

    if (RoomNameText)
    {
        RoomNameText->SetText(FText::FromString(InRoomName));
    }

    if (RoomTypeText)
    {
        FString RoomTypeStr;
        switch (InRoomType)
        {
        case EGameMode::MODE_DEATHMATCH :
            RoomTypeStr = TEXT("데스매치");
            break;
        case EGameMode::MODE_TEAM_DEATHMATCH:
            RoomTypeStr = TEXT("팀 데스매치");
            break;
        default:
            RoomTypeStr = TEXT("None");
        }
        RoomTypeText->SetText(FText::FromString(RoomTypeStr));
    }

    if (RoomPlayerCountText)
    {
        FString PlayerCountStr = FString::Printf(TEXT("%d/%d"), InCurrentPlayers, InMaxPlayers);
        RoomPlayerCountText->SetText(FText::FromString(PlayerCountStr));
    }
}
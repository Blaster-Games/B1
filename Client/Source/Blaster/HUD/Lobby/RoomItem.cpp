#include "RoomItem.h"
#include "Components/TextBlock.h"

void URoomItem::NativeConstruct()
{
    Super::NativeConstruct();

    if (RoomItemButton)
    {
        RoomItemButton->OnClicked.AddDynamic(this, &URoomItem::OnRoomClicked);
    }
}

void URoomItem::OnRoomItemButtonClicked()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float TimeSinceLastClick = CurrentTime - LastClickTime;

    // 현재 시간과 마지막 클릭 시간 로그
    UE_LOG(LogTemp, Log, TEXT("Current Time: %.3f, Last Click Time: %.3f"), CurrentTime, LastClickTime);
    UE_LOG(LogTemp, Log, TEXT("Time Since Last Click: %.3f, Double Click Time: %.3f"), TimeSinceLastClick, DoubleClickTime);

    if (TimeSinceLastClick <= DoubleClickTime)
    {
        UE_LOG(LogTemp, Log, TEXT("Double Click Detected! Broadcasting RoomId"));
        OnRoomItemClicked.Broadcast(RoomId);
        LastClickTime = 0.0f;
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Single Click Detected - Waiting for potential double click"));
        LastClickTime = CurrentTime;
    }
}

void URoomItem::OnRoomClicked()
{
    OnRoomItemClicked.Broadcast(RoomId);
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
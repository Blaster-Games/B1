#include "RoomItem.h"
#include "Components/TextBlock.h"

void URoomItem::NativeConstruct()
{
    Super::NativeConstruct();
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
            RoomTypeStr = TEXT("DeathMatch");
            break;
        case EGameMode::MODE_TEAM_DEATHMATCH:
            RoomTypeStr = TEXT("Team DeathMatch");
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
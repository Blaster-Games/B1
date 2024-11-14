#include "RoomChatMessage.h"
#include "Components/TextBlock.h"

void URoomChatMessage::NativeConstruct()
{
    Super::NativeConstruct();
}

void URoomChatMessage::SetChatMessage(const FString& PlayerName, const FString& Message)
{
    if (PlayerNameText)
    {
        PlayerNameText->SetText(FText::FromString(PlayerName));
    }

    if (RoomChatMessageText)
    {
        RoomChatMessageText->SetText(FText::FromString(Message));
    }
}
#include "RoomChatPanel.h"
#include "RoomChatMessage.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "GameInstance/BlasterNetworkSubsystem.h"

void URoomChatPanel::NativeConstruct()
{
    Super::NativeConstruct();

    if (RoomChatInputBox)
    {
        RoomChatInputBox->OnTextCommitted.AddDynamic(this, &URoomChatPanel::OnChatMessageCommitted);
    }

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>();
        if (NetworkSubsystem)
        {
            NetworkSubsystem->OnRoomChatMessage.AddDynamic(this, &URoomChatPanel::AddNewChatMessage);
        }
    }
}

void URoomChatPanel::NativeDestruct()
{
    if (NetworkSubsystem)
    {
        NetworkSubsystem->OnRoomChatMessage.RemoveDynamic(this, &URoomChatPanel::AddNewChatMessage);
    }

    Super::NativeDestruct();
}

void URoomChatPanel::AddNewChatMessage(int32 PlayerId, const FString& PlayerName, const FString& Message)
{
    if (!RoomChatScrollBox || !ChatMessageWidgetClass) return;

    URoomChatMessage* NewMessage = CreateWidget<URoomChatMessage>(this, ChatMessageWidgetClass);
    if (NewMessage)
    {
        NewMessage->SetChatMessage(PlayerName, Message);
        RoomChatScrollBox->AddChild(NewMessage);
        RoomChatScrollBox->ScrollToEnd();
    }
}

void URoomChatPanel::OnChatMessageCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        FString Message = Text.ToString();
        if (!Message.IsEmpty() && NetworkSubsystem)
        {
            // 네트워크 서브시스템을 통해 채팅 메시지 전송
            NetworkSubsystem->SendRoomChat(Message);

            // 입력창 비우기
            RoomChatInputBox->SetText(FText::GetEmpty());
        }
    }
}
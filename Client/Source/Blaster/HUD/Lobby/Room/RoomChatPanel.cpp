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
    UE_LOG(LogTemp, Log, TEXT("[AddNewChatMessage] Adding message from Player %d (%s)"), PlayerId, *PlayerName);

    if (!RoomChatScrollBox)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AddNewChatMessage] Failed - ScrollBox is null"));
        return;
    }

    if (!ChatMessageWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AddNewChatMessage] Failed - ChatMessageWidgetClass is null"));
        return;
    }

    URoomChatMessage* NewMessage = CreateWidget<URoomChatMessage>(this, ChatMessageWidgetClass);
    if (NewMessage)
    {
        UE_LOG(LogTemp, Log, TEXT("[AddNewChatMessage] Widget created successfully"));
        NewMessage->SetChatMessage(PlayerName, Message);
        RoomChatScrollBox->AddChild(NewMessage);
        RoomChatScrollBox->ScrollToEnd();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[AddNewChatMessage] Failed to create message widget"));
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

			RoomChatInputBox->SetKeyboardFocus();
        }
    }
}
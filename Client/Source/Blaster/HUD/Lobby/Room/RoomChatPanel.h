#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Framework/Text/TextLayout.h"
#include "RoomChatMessage.h"
#include "RoomChatPanel.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API URoomChatPanel : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:

	UFUNCTION()
	void AddNewChatMessage(int32 PlayerId, const FString& PlayerName, const FString& Message);

	UFUNCTION()
	void OnChatMessageCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat")
	TSubclassOf<URoomChatMessage> ChatMessageWidgetClass;


private:
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* RoomChatScrollBox;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* RoomChatInputBox;

	UPROPERTY()
	class UBlasterNetworkSubsystem* NetworkSubsystem;
};

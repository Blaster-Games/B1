

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomChatMessage.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API URoomChatMessage : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	void SetChatMessage(const FString& PlayerName, const FString& Message);

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RoomChatMessageText;
};

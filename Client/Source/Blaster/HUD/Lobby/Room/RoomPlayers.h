

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "../RoomTypes.h"
#include "RoomPlayers.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API URoomPlayers : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdatePlayers(const TArray<FPlayerInfo>& Players, int32 MaxPlayers);
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player1NameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player2NameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player3NameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player4NameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player5NameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player6NameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player7NameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player8NameText;

private:
	void InitializePlayerTextBlocks();
	void ClearAllSlots();
	bool IsRedTeamSlot(int32 SlotIndex) const { return SlotIndex < 4; }
	int32 GetTeamSlotIndex(int32 SlotIndex) const { return IsRedTeamSlot(SlotIndex) ? SlotIndex : SlotIndex - 4; }

	TArray<UTextBlock*> PlayerNameTexts;
};

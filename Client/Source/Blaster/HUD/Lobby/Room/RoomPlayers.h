

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
	// 플레이어 정보 업데이트
	void UpdatePlayers(const TArray<FPlayerInfo>& Players);
	
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
	// TextBlock 배열로 관리하면 편리
	void InitializePlayerTextBlocks();
	TArray<UTextBlock*> PlayerNameTexts;
};

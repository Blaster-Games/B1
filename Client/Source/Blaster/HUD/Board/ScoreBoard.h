

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/ScoreStruct.h"
#include "ScoreBoard.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UScoreBoard : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateScoreboard(const TArray<FPlayerScoreData>& PlayerScoreData);

private:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* PlayerList;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UPlayerScore> PlayerScoreClass;

	void SortPlayersByKills(TArray<FPlayerScoreData>& PlayerScoreData);
};

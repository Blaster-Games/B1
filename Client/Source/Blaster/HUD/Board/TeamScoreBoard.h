

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/ScoreStruct.h"
#include "TeamScoreBoard.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UTeamScoreBoard : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void UpdateTeamScoreboard(const TArray<FTeamPlayerScoreData>& TeamPlayerScoreData, float RedScore, float BlueScore);



private:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RedTeamScore;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BlueTeamScore;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* RedTeamList;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* BlueTeamList;

    UPROPERTY(EditAnywhere, Category = "HUD")
    TSubclassOf<class UTeamPlayerScore> TeamPlayerScoreClass;

    void SortPlayersByKills(TArray<FTeamPlayerScoreData>& TeamPlayerScoreData);
};

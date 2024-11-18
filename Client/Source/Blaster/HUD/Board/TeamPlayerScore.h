

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/ScoreStruct.h"
#include "TeamPlayerScore.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UTeamPlayerScore : public UUserWidget
{
	GENERATED_BODY()
	

public:
    virtual void NativeConstruct() override;
    void SetPlayerData(const FTeamPlayerScoreData& Data);

private:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* NameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* KillText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DeathText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CoinText;


};

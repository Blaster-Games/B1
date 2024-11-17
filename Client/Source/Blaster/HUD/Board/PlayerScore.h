

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/ScoreStruct.h"
#include "PlayerScore.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UPlayerScore : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;
    void SetPlayerData(const FPlayerScoreData& Data);

private:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* NameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* KillText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DeathText;
};

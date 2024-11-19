#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"
/**
 *
 */
UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* DisplayText;

    void SetDisplayText(FString TextToDisplay);

    UFUNCTION(BlueprintCallable)
    void ShowPlayerNetRole(APawn* InPawn);

    void UpdateVisibility(APawn* LocalPawn, APawn* TargetPawn);

protected:
    // (버전에 따른 변경) Override NativeDestruct instead of OnLevelRemovedFromWorld 
    virtual void NativeDestruct() override;
};
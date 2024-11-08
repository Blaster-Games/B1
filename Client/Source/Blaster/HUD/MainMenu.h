

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* EnterLobbyText;

    UPROPERTY(meta = (BindWidget))
    UButton* EnterLobbyButton;

    UFUNCTION()
    void OnEnterLobbyButtonClicked();
};

// EnterMainMenu.h
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "EnterMainMenu.generated.h"

UCLASS()
class BLASTER_API UEnterMainMenu : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* EnterMainMenuText;

    UPROPERTY(meta = (BindWidget))
    UButton* EnterButton;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSoftClassPtr<UUserWidget> MainMenuWidgetClass;

    UFUNCTION()
    void OnEnterButtonClicked();

    UFUNCTION()
    void HandleEnterSuccess();

    UFUNCTION()
    void HandleEnterFailed(const FString& ErrorMessage);
};
// Login.h
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Login.generated.h"

UCLASS()
class BLASTER_API ULogin : public UUserWidget
{
    GENERATED_BODY()

protected:
    UFUNCTION()
    void HandleLoginSuccess();

    UFUNCTION()
    void HandleLoginFailed(const FString& ErrorMessage);

    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* EmailTextBox;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* PasswordTextBox;

    UPROPERTY(meta = (BindWidget))
    UButton* LoginButton;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSoftClassPtr<UUserWidget> EnterMainMenuWidgetClass;

    UFUNCTION()
    void OnLoginButtonClicked();
};
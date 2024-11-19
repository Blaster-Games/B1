

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReturnToMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UReturnToMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void MenuSetup();
	void MenuTearDown();

protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnPlayerLeftGame();

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* OptionButton;

	UFUNCTION()
	void OptionButtonClicked();

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class USetting> SettingWidgetClass;

	UPROPERTY()
	USetting* SettingWidget;


	UPROPERTY(meta = (BindWidget))
	UButton* ReturnButton;

	UFUNCTION()
	void ReturnButtonClicked();

	UPROPERTY()
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY() 
	class APlayerController* PlayerController; // 자주 쓰일 것 같아서 이렇게 뺌.
};

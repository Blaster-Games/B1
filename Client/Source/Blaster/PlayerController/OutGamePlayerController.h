

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HUD/Lobby/Lobby.h"
#include "OutGamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AOutGamePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    AOutGamePlayerController();

    virtual void BeginPlay() override;

    void ShowLobbyWidget();

protected:
    // UI 입력 모드 설정
    UFUNCTION(BlueprintCallable)
    void SetupUIInputMode();

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<ULobby> LobbyWidgetClass;

    UPROPERTY()
    ULobby* CurrentLobbyWidget;
};



#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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

protected:
    // UI 입력 모드 설정
    UFUNCTION(BlueprintCallable)
    void SetupUIInputMode();
};

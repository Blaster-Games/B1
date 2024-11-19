

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "BlasterSaveSettings.generated.h"

USTRUCT(BlueprintType)
struct FSensitivitySettings
{
    GENERATED_BODY()

    UPROPERTY()
    float Sensitivity = 6.0f;

    UPROPERTY()
    float AimSensitivity = 6.0f;

    UPROPERTY()
    float ScopedSensitivity = 6.0f;
};

/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterSaveSettings : public USaveGame
{
	GENERATED_BODY()
	
public:
    UPROPERTY()
    FSensitivitySettings SensitivitySettings;

    static const FString SaveSlotName;
    static const int32 UserIndex;
};

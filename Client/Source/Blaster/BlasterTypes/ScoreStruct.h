#pragma once

#include "CoreMinimal.h"
#include "Blaster/BlasterTypes/Team.h"
#include "ScoreStruct.generated.h"

USTRUCT(BlueprintType)
struct FPlayerScoreData
{
    GENERATED_BODY()

    UPROPERTY()
    FString PlayerName;

    UPROPERTY()
    int32 Kill;

    UPROPERTY()
    int32 Death;
};


USTRUCT(BlueprintType)
struct FTeamPlayerScoreData
{
    GENERATED_BODY()

    UPROPERTY()
    FString PlayerName;

    UPROPERTY()
    int32 Kill;

    UPROPERTY()
    int32 Death;

    UPROPERTY()
    int32 Coin;

    UPROPERTY()
    ETeam Team;

};
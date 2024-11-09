#pragma once
#include "CoreMinimal.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/BuffTypes.h"
#include "Blaster/BlasterTypes/ThrowTypes.h"
#include "ShopStructs.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Price;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D> WeaponImage;


    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EWeaponType WeaponType;
};

USTRUCT(BlueprintType)
struct FThrowData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ThrowName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Price;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D> ThrowImage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EThrowType ThrowType;
};


USTRUCT(BlueprintType)
struct FBuffData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString BuffName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Price;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D> BuffImage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBuffType BuffType;
};
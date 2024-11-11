#pragma once

UENUM(BlueprintType)
enum class EBuffType : uint8
{
    
    EBT_None UMETA(DisplayName = "None"),
    EBT_Health UMETA(DisplayName = "Health Buff"),
    EBT_Shield UMETA(DisplayName = "Shield Buff"),
    EBT_Speed UMETA(DisplayName = "Speed Buff"),
    EBT_Jump UMETA(DisplayName = "Jump Buff"),


    EBT_MAX UMETA(DisplayName = "DefaultMAX")
};
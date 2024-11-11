#pragma once
#include "CoreMinimal.h"
#include "RoomTypes.generated.h"

UENUM(BlueprintType)
enum class EGameMode : uint8
{
	MODE_NONE = 0 UMETA(DisplayName = "None"),
	MODE_DEATHMATCH = 1 UMETA(DisplayName = "DeathMatch"),
	MODE_TEAM_DEATHMATCH = 2 UMETA(DisplayName = "TeamDeathMatch"),
};

UENUM(BlueprintType)
enum class ERoomState : uint8
{
	STATE_NONE = 0 UMETA(DisplayName = "None"),
	STATE_WAITING = 1 UMETA(DisplayName = "Waiting"),
	STATE_PLAYING = 2 UMETA(DisplayName = "Playing"),
};

USTRUCT(BlueprintType)
struct FRoomListItemInfo
{
    GENERATED_BODY()
    
	UPROPERTY()
	int32 RoomId;

	UPROPERTY()
	FString RoomName;

	UPROPERTY()
	EGameMode RoomType;

	UPROPERTY()
	int32 CurrentPlayers;

	UPROPERTY()
	int32 MaxPlayers;

	UPROPERTY()
	ERoomState State;

	UPROPERTY()
	FString MapName;

	// 기본 생성자
	FRoomListItemInfo()
		: RoomId(0)
		, RoomType(EGameMode::MODE_NONE)
		, CurrentPlayers(0)
		, MaxPlayers(0)
		, State(ERoomState::STATE_NONE)
	{
	}
};
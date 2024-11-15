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

// RoomTypes.h에 추가

UENUM(BlueprintType)
enum class ETeamType : uint8
{
	TEAM_NONE = 0 UMETA(DisplayName = "None"),
	TEAM_RED = 1 UMETA(DisplayName = "Red"),
	TEAM_BLUE = 2 UMETA(DisplayName = "Blue"),
};

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int32 PlayerId;

	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	bool IsHost;

	UPROPERTY()
	ETeamType Team;

	// 기본 생성자
	FPlayerInfo()
		: PlayerId(0)
		, IsHost(false)
		, Team(ETeamType::TEAM_NONE)
	{
	}
};

USTRUCT(BlueprintType)
struct FRoomDetailInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int32 RoomId;

	UPROPERTY()
	FString RoomName;

	UPROPERTY()
	EGameMode RoomType;

	UPROPERTY()
	int32 MaxPlayers;

	UPROPERTY()
	TArray<FPlayerInfo> Players;

	UPROPERTY()
	ERoomState State;

	UPROPERTY()
	FString MapName;

	UPROPERTY()
	int32 HostPlayerId;

	// 기본 생성자
	FRoomDetailInfo()
		: RoomId(0)
		, RoomType(EGameMode::MODE_NONE)
		, MaxPlayers(0)
		, State(ERoomState::STATE_NONE)
		, HostPlayerId(0)
	{
	}
};


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

// 사용자 지정 만들기
namespace MatchState
{
	extern BLASTER_API const FName Cooldown; // Match duration has been reached. Display winner and begin cooldown timer.
}

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABlasterGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class AMyBlasterCharacter* ElimmedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	void PlayerLeftGame(class ABlasterPlayerState* PlayerLeaving);
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;

	bool bTeamsMatch = false;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

	// 라운드 관련
	virtual void ResetAllPlayers();
	virtual void StartNewRound();
	virtual void EndRound();

	UPROPERTY(EditDefaultsOnly, Category = "Game Rules")
	bool bIsRoundBased = false;

	UPROPERTY(EditDefaultsOnly, Category = "Game Rules")
	int32 MaxRounds = 5;

	float CountdownTime = 0.f;

public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
	FORCEINLINE bool IsRoundBased() const { return bIsRoundBased; }
};


// GameMode는 GameModeBase와 달리 Match States(매치 상태)와 관련한 기능이 더 있다.
// 게임 모드에는 매치 상태 뿐 아니라 매치 상태가 바뀔 때 사용할 수 있는 기능도 있다.


// 원래는 Round를 따로 상속 받아서 하는 방식으로 했지만, 뭔가 여기에다가 그냥 라운드 개념을 추가하는 방식이 
// 좀 더 좋은 것 같아서 바꿈.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class ABlasterPlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<ABlasterPlayerState*> TopScoringPlayers;

	/**
	* Teams
	*/

	void RedTeamScores();
	void BlueTeamScores();


	TArray<ABlasterPlayerState*> RedTeam;
	TArray<ABlasterPlayerState*> BlueTeam;

	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
	float RedTeamScore = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
	float BlueTeamScore = 0.f;

	UFUNCTION()
	void OnRep_RedTeamScore();

	UFUNCTION()
	void OnRep_BlueTeamScore();

private:
	float TopScore = 0.f;


	// Rounds 관련
	UPROPERTY(ReplicatedUsing = OnRep_CurrentRound)
	int32 CurrentRound = 1;

	UPROPERTY(Replicated)
	int32 MaxRounds = 5;
	
protected:

	UFUNCTION()
	void OnRep_CurrentRound();

public:
	FORCEINLINE int32 GetCurrentRound() const { return CurrentRound; }
	FORCEINLINE int32 GetMaxRounds() const { return MaxRounds; }

	void SetCurrentRound(int32 NewRound);
	void SetMaxRounds(int32 NewMaxRounds);

};



// (참고)
// 게임 모드가 base 기반이 아니므로 GameState도 base 기반이 아닌 GameState로
// GameState는 PlayerState와 매우 비슷하지만 게임 전체의 상태에 더 가깝다.

// 강의에선 이게 팀 게임인지에 대한 정보를 여기에 저장안하고 햇는데, 그것보단 여기서 관리하는 게 좋아보임.
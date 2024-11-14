


#include "GameMode/BlasterGameMode.h"
#include "Blaster/Character/MyBlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/BlasterComponents/BuffComponent.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "GameFramework/GameState.h" 


namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true; // 시작 상태를 기다림. - 수동으로 매치 시작을 할 때까지 옵저버 처럼 맵 탐방 할 수 있는 정도의 상태을 유지.
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (bIsRoundBased)
	{
		if (ABlasterGameState* BlasterGS = GetGameState<ABlasterGameState>())
		{
			BlasterGS->SetMaxRounds(MaxRounds);
		}
	}

	KillReward = CalculateKillReward();

}


void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - (CurrentTime - StateStartTime);

		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = MatchTime - (CurrentTime - StateStartTime);
		if (CountdownTime <= 0.f)
		{
			if (bIsRoundBased)
			{
				EndRound();
			}
			else
			{
				SetMatchState(MatchState::Cooldown);
			}
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime - (CurrentTime - StateStartTime);
		if (CountdownTime <= 0.f)
		{
			if (bIsRoundBased)
			{
				ABlasterGameState* BlasterGS = GetGameState<ABlasterGameState>();
				if (BlasterGS && BlasterGS->GetCurrentRound() < BlasterGS->GetMaxRounds())
				{
					StartNewRound();
				}
				else
				{
					RestartGame();
				}
			}
			else
			{
				RestartGame();
			}
		}
	}
}


void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	StateStartTime = GetWorld()->GetTimeSeconds();

	// 게임에 있는 모든 플레이어 컨트롤러를 가져와서 매치 상태를 알릴 수 있음.
	// 플레이어 컨트롤러를 모두 모으려면 Iterator를 사용해야됨.
	// 서버의 모든 플레이어 컨트롤러를 순환하고 매치 상태를 설정함.
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer)
		{
			BlasterPlayer->OnMatchStateSet(MatchState, bTeamsMatch);
		}
	}
}

void ABlasterGameMode::ResetAllPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (!BlasterPlayer) continue;

		if (AMyBlasterCharacter* PlayerCharacter = Cast<AMyBlasterCharacter>(BlasterPlayer->GetPawn()))
		{
			PlayerCharacter->Reset(); //  영혼 추출
			PlayerCharacter->Destroy(); // 기존 캐릭터의 육체 제거
		}

		// 리스폰
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		if (PlayerStarts.Num() > 0)
		{
			int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
			RestartPlayerAtPlayerStart(BlasterPlayer, PlayerStarts[Selection]); // 새로운 육체 생성 + 영혼 이전.
		}
	}

	// 여기서 컴포넌트 초기화도 해줘야 된다???
}

void ABlasterGameMode::StartNewRound()
{
	ABlasterGameState* BlasterGS = GetGameState<ABlasterGameState>();
	if (BlasterGS)
	{
		int32 NewRound = BlasterGS->GetCurrentRound() + 1;
		BlasterGS->SetCurrentRound(NewRound);
	}

	ResetAllPlayers();
	AllPlayerApplyBuffs();
	
	SetMatchState(MatchState::InProgress);
}

void ABlasterGameMode::EndRound()
{
	if (ABlasterGameState* BlasterGS = GetGameState<ABlasterGameState>())
	{
		// 팀별 생존자 수 카운트
		int32 RedTeamAlive = 0;
		int32 BlueTeamAlive = 0;

		for (APlayerState* PS : BlasterGS->PlayerArray)
		{
			if (ABlasterPlayerState* BPS = Cast<ABlasterPlayerState>(PS))
			{
				// 라운드 종료 보상 지급
				BPS->SetMoney(BPS->GetMoney() + RoundReward);

				// 버프 초기화
				BPS->ClearBuff();

				if (AMyBlasterCharacter* Character = Cast<AMyBlasterCharacter>(BPS->GetPawn()))
				{
					// 수류탄 개수 저장
					if (UCombatComponent* Combat = Character->GetCombat())
					{
						Combat->SaveGrenadeCount();
					}

					// 살아있는 팀원 카운트
					if (!Character->IsElimmed())
					{
						if (BPS->GetTeam() == ETeam::ET_RedTeam)
						{
							RedTeamAlive++;
						}
						else if (BPS->GetTeam() == ETeam::ET_BlueTeam)
						{
							BlueTeamAlive++;
						}
					}
				}
			}
		}

		// 승자 결정 및 라운드 점수 업데이트
		if (RedTeamAlive > BlueTeamAlive)
		{
			BlasterGS->RedTeamScores();
		}
		else if (BlueTeamAlive > RedTeamAlive)
		{
			BlasterGS->BlueTeamScores();
		}
	}	
	SetMatchState(MatchState::Cooldown);
}

void ABlasterGameMode::AllPlayerApplyBuffs()
{
	ABlasterGameState* BlasterGS = GetGameState<ABlasterGameState>();
	if (BlasterGS)
	{
		for (APlayerState* PS : BlasterGS->PlayerArray)
		{
			if (ABlasterPlayerState* BPS = Cast<ABlasterPlayerState>(PS))
			{
				// 구매한 버프들 적용
				if (AMyBlasterCharacter* Character = Cast<AMyBlasterCharacter>(BPS->GetPawn()))
				{
					if (UBuffComponent* BuffComp = Character->GetBuff())
					{
						TArray<EBuffType> ActiveBuffs = BPS->GetActiveBuffs();
						for (EBuffType Buff : ActiveBuffs)
						{
							BuffComp->ApplyBuff(Buff);
						}
					}
				}
			}
		}
	}
}

bool ABlasterGameMode::IsTeamEliminated(ETeam Team) const
{
	ABlasterGameState* BlasterGS = GetGameState<ABlasterGameState>();
	if (!BlasterGS) return false;

	for (APlayerState* PS : BlasterGS->PlayerArray)
	{
		if (ABlasterPlayerState* BPS = Cast<ABlasterPlayerState>(PS))
		{
			if (BPS->GetTeam() == Team)
			{
				if (AMyBlasterCharacter* Character = Cast<AMyBlasterCharacter>(BPS->GetPawn()))
				{
					if (!Character->IsElimmed())
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}


int32 ABlasterGameMode::GetTotalPlayerCount() const
{
	ABlasterGameState* BlasterGS = GetGameState<ABlasterGameState>();
	return BlasterGS ? BlasterGS->PlayerArray.Num() : 0;
}


int32 ABlasterGameMode::CalculateKillReward()
{
	int32 TotalPlayers = GetTotalPlayerCount();
	float RewardMultiplier = 1.0f;

	// 플레이어 수가 적을수록 더 높은 보상
	if (TotalPlayers <= 4)  // 2v2 이하
	{
		RewardMultiplier = 2.0f;  // 2배 보상
	}
	else if (TotalPlayers <= 6)  // 3v3
	{
		RewardMultiplier = 1.5f;  // 1.5배 보상
	}
	// 4v4 이상은 기본 보상

	return FMath::RoundToInt(KillReward * RewardMultiplier);
}


bool ABlasterGameMode::ShouldRespawnPlayer() const
{
	// 라운드 기반 게임 이고 매치가 진행 중이면 리스폰하지 않음
	if (ABlasterGameState* BlasterGS = GetGameState<ABlasterGameState>())
	{
		return !(bIsRoundBased && MatchState == MatchState::InProgress);
	}
	return true;
}


float ABlasterGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}



// 서버에서만 실행됨.
void ABlasterGameMode::PlayerEliminated(AMyBlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGameState)
	{
		// 갱신 전 선두 리스트
		TArray<ABlasterPlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : BlasterGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		AttackerPlayerState->AddToScore(1.f);
		AttackerPlayerState->SetMoney(AttackerPlayerState->GetMoney() + KillReward);

		BlasterGameState->UpdateTopScore(AttackerPlayerState);
		if (BlasterGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			// 선두 차지
			AMyBlasterCharacter* Leader = Cast<AMyBlasterCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedTheLead();
			}
		}

		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{
			if (!BlasterGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				// 선두를 뺏김.
				AMyBlasterCharacter* Loser = Cast<AMyBlasterCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
				if (Loser)
				{
					Loser->MulticastLostTheLead();
				}
			}
		}
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false); // 유저가 떠나진 않고 그냥 죽은 상태
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer && AttackerPlayerState && VictimPlayerState)
		{
			BlasterPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
		}
	}

	if (bIsRoundBased)
	{
		bool bRedTeamEliminated = IsTeamEliminated(ETeam::ET_RedTeam);
		bool bBlueTeamEliminated = IsTeamEliminated(ETeam::ET_BlueTeam);

		if (bRedTeamEliminated || bBlueTeamEliminated)
		{
			EndRound();
		}
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (!ShouldRespawnPlayer())
	{
		return;
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset(); // 컨트롤러에서 캐릭터를 분리하고 컨트롤러에 대한 소유권을 호출 
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		// 모든 actor를 가져오고 월드의 모든 플레이서 시작에 대한 포인터로 해당 배열을 채울 것임.
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}

// 떠난 플레이어의 상태 정보를 저장하는 객체를 가리키는 포인터
void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeaving)
{
	// call elim, passing in true for bLeftGame
	if (PlayerLeaving == nullptr) return;
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		BlasterGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	AMyBlasterCharacter* CharacterLeaving = Cast<AMyBlasterCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Elim(true); // 정말로 유저가 나간 case로 아예 제거 처리할 것.
	}
}


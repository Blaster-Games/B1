


#include "GameMode/BlasterGameMode.h"
#include "Blaster/Character/MyBlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"

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
	// Blaster 게임모드는 게임 시작 맵이 아닌 Blaster 맵에서만 사용됨.
	// -> 따라서 게임을 시작할 때부터 Blaster 맵에 실제로 들어가기까지 얼마나 많은 시간이 걸렸는지 알 수 있음.
	LevelStartingTime = GetWorld()->GetTimeSeconds();


}


void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
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
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
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
	//// 현재 레벨의 모든 플레이어 컨트롤러를 가져옴
	//for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	//{
	//	ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
	//	if (BlasterPlayer && BlasterPlayer->GetPawn())
	//	{
	//		// 플레이어의 캐릭터를 가져옴
	//		AMyBlasterCharacter* PlayerCharacter = Cast<AMyBlasterCharacter>(BlasterPlayer->GetPawn());
	//		if (PlayerCharacter)
	//		{
	//			// 캐릭터 상태 초기화
	//			PlayerCharacter->Reset(); // 캐릭터의 상태를 초기화합니다.

	//			// 모든 APlayerStart를 가져옵니다.
	//			TArray<AActor*> PlayerStarts;
	//			UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

	//			if (PlayerStarts.Num() > 0)
	//			{
	//				// 랜덤한 플레이어 시작 위치를 선택합니다.
	//				int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);

	//				// 플레이어를 리스폰합니다.
	//				RestartPlayerAtPlayerStart(BlasterPlayer, PlayerStarts[Selection]);

	//				// 이제 이전 캐릭터를 파괴합니다.
	//				PlayerCharacter->Destroy();
	//			}
	//		}
	//	}
	//}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (!BlasterPlayer) continue;

		if (AMyBlasterCharacter* PlayerCharacter = Cast<AMyBlasterCharacter>(BlasterPlayer->GetPawn()))
		{
			PlayerCharacter->Reset();
			PlayerCharacter->Destroy();
		}

		// 리스폰
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		if (PlayerStarts.Num() > 0)
		{
			int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
			RestartPlayerAtPlayerStart(BlasterPlayer, PlayerStarts[Selection]);
		}
	}
}

void ABlasterGameMode::StartNewRound()
{
	ABlasterGameState* BlasterGS = GetGameState<ABlasterGameState>();
	if (BlasterGS)
	{
		int32 NewRound = BlasterGS->GetCurrentRound() + 1;
		BlasterGS->SetCurrentRound(NewRound);
	}

	LevelStartingTime = GetWorld()->GetTimeSeconds();
	ResetAllPlayers();
	SetMatchState(MatchState::InProgress);
}

void ABlasterGameMode::EndRound()
{
	// 라운드 종료 처리 (승자 결정 등)
	if (ABlasterGameState* BlasterGS = GetGameState<ABlasterGameState>())
	{
		//
	}

	SetMatchState(MatchState::Cooldown);
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
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
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


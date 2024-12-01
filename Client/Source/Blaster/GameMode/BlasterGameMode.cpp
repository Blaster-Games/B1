


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
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/GameInstance/BlasterGameInstance.h"


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

void ABlasterGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// 전근렬이 잘못짠 코드
	//if (UBlasterGameInstance* GameInstance = Cast<UBlasterGameInstance>(GetGameInstance()))
	//{
	//	if (ABlasterPlayerState* PS = NewPlayer->GetPlayerState<ABlasterPlayerState>())
	//	{
	//		FString PlayerNickname = GameInstance->GetNickname();
	//		// 닉네임 설정
	//		PS->SetNickname(PlayerNickname); 
	//	}
	//}
}

void ABlasterGameMode::RestartGame()
{
	// 서버에서 모든 플레이어 컨트롤러에게 메인 메뉴로 돌아가라고 알림
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer)
		{
			BlasterPlayer->ReturnToMainMenuAfterMatch();
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


void ABlasterGameMode::RestartPlayer(AController* NewPlayer)
{
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	AActor* SpawnPoint = FindSafestSpawnPoint();
	if (SpawnPoint == nullptr)
	{
		// 안전한 스폰 포인트를 찾지 못했다면 이전 스폰 포인트 사용
		if (NewPlayer->StartSpot != nullptr)
		{
			SpawnPoint = NewPlayer->StartSpot.Get();
			UE_LOG(LogGameMode, Warning, TEXT("RestartPlayer: Safe spawn point not found, using last start spot"));
		}
	}

	RestartPlayerAtPlayerStart(NewPlayer, SpawnPoint);
}

void ABlasterGameMode::ResetAllPlayers()
{
	ABlasterGameState* BlasterGS = GetGameState<ABlasterGameState>();
	if (!BlasterGS) return;

	for (APlayerState* PlayerState : BlasterGS->PlayerArray)
	{
		if (!PlayerState) continue;

		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(PlayerState->GetPlayerController());
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
	CheckWeaponSlots();
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
		AActor* SpawnPoint = FindSafestSpawnPoint();
		RestartPlayerAtPlayerStart(ElimmedController, SpawnPoint);
	}
}

AActor* ABlasterGameMode::FindSafestSpawnPoint()
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

	if (PlayerStarts.Num() == 0) return nullptr;

	// 안전한 스폰 포인트들을 저장할 배열
	TArray<AActor*> SafeSpawnPoints;
	const float SafeRadius = 1000.f;

	// 각 스폰 포인트 검사
	for (AActor* Start : PlayerStarts)
	{
		bool bIsSafe = true;

		// 주변 캐릭터 체크
		TArray<AActor*> NearbyCharacters;
		UGameplayStatics::GetAllActorsOfClass(this, AMyBlasterCharacter::StaticClass(), NearbyCharacters);

		for (AActor* Actor : NearbyCharacters)
		{
			AMyBlasterCharacter* Character = Cast<AMyBlasterCharacter>(Actor);
			if (!Character || Character->IsElimmed()) continue;

			// 거리 계산
			float Distance = FVector::Dist(Start->GetActorLocation(), Character->GetActorLocation());
			if (Distance < SafeRadius)
			{
				bIsSafe = false;
				break;
			}
		}

		if (bIsSafe)
		{
			SafeSpawnPoints.Add(Start);
		}
	}

	// 안전한 스폰 포인트 중 랜덤 선택
	if (SafeSpawnPoints.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, SafeSpawnPoints.Num() - 1);
		return SafeSpawnPoints[Selection];
	}

	// 안전한 지점이 없다면 기존처럼 랜덤 선택
	int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
	return PlayerStarts[Selection];
}

void ABlasterGameMode::CheckWeaponSlots()
{
	ABlasterGameState* BlasterGS = GetGameState<ABlasterGameState>();
	if (!BlasterGS) return;

	// GameState에서 모든 플레이어의 무기 슬롯을 확인
	for (APlayerState* PS : BlasterGS->PlayerArray)
	{
		if (ABlasterPlayerState* BlasterPS = Cast<ABlasterPlayerState>(PS))
		{
			FWeaponSlots WeaponSlots = BlasterPS->GetWeaponSlots();

			// 슬롯 1 체크
			if (WeaponSlots.Slot1Weapon != EWeaponType::EWT_MAX)
			{
				BlasterGS->AddWeaponPurchase(WeaponSlots.Slot1Weapon);
			}

			// 슬롯 2 체크
			if (WeaponSlots.Slot2Weapon != EWeaponType::EWT_MAX)
			{
				BlasterGS->AddWeaponPurchase(WeaponSlots.Slot2Weapon);
			}
		}
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


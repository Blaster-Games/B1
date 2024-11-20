


#include "TeamsGameMode.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/BlasterGameInstance.h"


ATeamsGameMode::ATeamsGameMode()
{
	bTeamsMatch = true;
}

// 도중 난입 (PlayerInfo를 통해 팀 선택하는 부분으로 변경해줘야함. 근데 도중난입 안만들거니깐 안넣음)
void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));

	if (BGameState)
	{
		ABlasterPlayerState* BPState = NewPlayer->GetPlayerState<ABlasterPlayerState>();
		if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
			{
				BGameState->RedTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				BGameState->BlueTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
}

void ATeamsGameMode::Logout(AController* Exiting)
{
	ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	ABlasterPlayerState* BPState = Exiting->GetPlayerState<ABlasterPlayerState>();
	if (BGameState && BPState)
	{
		if (BGameState->RedTeam.Contains(BPState))
		{
			BGameState->RedTeam.Remove(BPState);
		}
		if (BGameState->BlueTeam.Contains(BPState))
		{
			BGameState->BlueTeam.Remove(BPState);
		}
	}
}

float ATeamsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	ABlasterPlayerState* AttackerPState = Attacker->GetPlayerState<ABlasterPlayerState>();
	ABlasterPlayerState* VictomPState = Victim->GetPlayerState<ABlasterPlayerState>();
	if (AttackerPState == nullptr || VictomPState == nullptr) return BaseDamage;
	// 공격자와 피해자가 동일하다면
	if (VictomPState == AttackerPState)
	{
		return BaseDamage;
	}
	if (AttackerPState->GetTeam() == VictomPState->GetTeam())
	{
		return 0.f;
	}

	return BaseDamage;

}

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));

	// 여기부터 팀분배로직 변경됨
	UBlasterGameInstance* GameInstance = Cast<UBlasterGameInstance>(GetGameInstance());

	if (BGameState && GameInstance)
	{
		// 현재 방의 정보를 가져옴
		const FRoomDetailInfo& CurrentRoom = GameInstance->GetCurrentRoomInfo();

		for (auto PState : BGameState->PlayerArray)
		{
			ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(PState.Get());
			if (BPState)
			{
				// 플레이어의 ID나 이름으로 매칭하여 FPlayerInfo를 찾음
				const FPlayerInfo* PlayerInfo = CurrentRoom.Players.FindByPredicate([&](const FPlayerInfo& Info) {
					return Info.PlayerName == BPState->GetPlayerName();
					});

				if (PlayerInfo)
				{
					// FPlayerInfo의 TeamType에 따라 팀 할당
					if (PlayerInfo->Team == ETeamType::TEAM_RED)
					{
						BGameState->RedTeam.AddUnique(BPState);
						BPState->SetTeam(ETeam::ET_RedTeam);
					}
					else if (PlayerInfo->Team == ETeamType::TEAM_BLUE)
					{
						BGameState->BlueTeam.AddUnique(BPState);
						BPState->SetTeam(ETeam::ET_BlueTeam);
					}

				}
			}
		}
	}

	/*if (BGameState)
	{
		for (auto PState : BGameState->PlayerArray)
		{
			ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(PState.Get());
			if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
				{
					BGameState->RedTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					BGameState->BlueTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}*/
}

// me
void ATeamsGameMode::PlayerEliminated(AMyBlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);

	ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;


	if (BGameState == nullptr || AttackerPlayerState == nullptr || VictimPlayerState == nullptr)
	{
		return;
	}

	// 자살인 경우 점수 업데이트 하지 않고 return (me)
	if (AttackerPlayerState == VictimPlayerState)
	{
		return;
	}

	// 팀 점수 업데이트 (라운드 기반이 아닐 경우)
	if (!bIsRoundBased)
	{
		if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			BGameState->BlueTeamScores();
		}
		else if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			BGameState->RedTeamScores();
		}
	}
	
}
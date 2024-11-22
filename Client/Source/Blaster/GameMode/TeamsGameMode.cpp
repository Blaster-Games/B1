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

    UBlasterGameInstance* GameInstance = GetGameInstance<UBlasterGameInstance>();
    ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));

    if (BGameState && GameInstance && HasAuthority())
    {
        const FRoomDetailInfo& HostRoomInfo = GameInstance->GetCurrentRoomInfo();

        // 현재 상태 로깅
        UE_LOG(LogTemp, Log, TEXT("Starting team assignment - Room has %d players, GameState has %d players"),
            HostRoomInfo.Players.Num(), BGameState->PlayerArray.Num());

        // 먼저 RoomInfo의 플레이어 정보 출력
        UE_LOG(LogTemp, Log, TEXT("=== Room Info Players ==="));
        for (const FPlayerInfo& RoomPlayer : HostRoomInfo.Players)
        {
            UE_LOG(LogTemp, Log, TEXT("Room Player - Name: %s, Team: %s"),
                *RoomPlayer.PlayerName,
                RoomPlayer.Team == ETeamType::TEAM_RED ? TEXT("Red") : TEXT("Blue"));
        }

        // 현재 PlayerState 정보 출력
        UE_LOG(LogTemp, Log, TEXT("=== Current PlayerStates ==="));
        for (auto PState : BGameState->PlayerArray)
        {
            if (ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(PState.Get()))
            {
                UE_LOG(LogTemp, Log, TEXT("PlayerState - Name: %s, Current Team: %d"),
                    *BPState->GetPlayerName(),
                    static_cast<int32>(BPState->GetTeam()));
            }
        }

        // 팀 배정 시작
        int32 ProcessedPlayers = 0;
        for (auto PState : BGameState->PlayerArray)
        {
            if (ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(PState.Get()))
            {
                if (ProcessedPlayers < HostRoomInfo.Players.Num())
                {
                    const FPlayerInfo& RoomPlayer = HostRoomInfo.Players[ProcessedPlayers];

                    // 닉네임 설정
                    BPState->SetNickname(RoomPlayer.PlayerName);

                    UE_LOG(LogTemp, Log, TEXT("Assigning player %s to team %s"),
                        *RoomPlayer.PlayerName,
                        RoomPlayer.Team == ETeamType::TEAM_RED ? TEXT("Red") : TEXT("Blue"));

                    // 팀 설정
                    if (RoomPlayer.Team == ETeamType::TEAM_RED)
                    {
                        BGameState->RedTeam.AddUnique(BPState);
                        BPState->SetTeam(ETeam::ET_RedTeam);
                        UE_LOG(LogTemp, Log, TEXT("Added to Red Team: %s"), *RoomPlayer.PlayerName);
                    }
                    else if (RoomPlayer.Team == ETeamType::TEAM_BLUE)
                    {
                        BGameState->BlueTeam.AddUnique(BPState);
                        BPState->SetTeam(ETeam::ET_BlueTeam);
                        UE_LOG(LogTemp, Log, TEXT("Added to Blue Team: %s"), *RoomPlayer.PlayerName);
                    }

                    ProcessedPlayers++;
                }
            }
        }

        // 최종 결과 확인
        UE_LOG(LogTemp, Log, TEXT("=== Final Team Assignment ==="));
        UE_LOG(LogTemp, Log, TEXT("Red Team: %d players"), BGameState->RedTeam.Num());
        for (auto Player : BGameState->RedTeam)
        {
            if (ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(Player))
            {
                UE_LOG(LogTemp, Log, TEXT("Red Team Player: %s"), *BPState->GetNickname());
            }
        }
        UE_LOG(LogTemp, Log, TEXT("Blue Team: %d players"), BGameState->BlueTeam.Num());
        for (auto Player : BGameState->BlueTeam)
        {
            if (ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(Player))
            {
                UE_LOG(LogTemp, Log, TEXT("Blue Team Player: %s"), *BPState->GetNickname());
            }
        }
    }
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
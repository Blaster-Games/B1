


#include "GameState/BlasterGameState.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"


void ABlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterGameState, TopScoringPlayers);
	DOREPLIFETIME(ABlasterGameState, RedTeamScore);
	DOREPLIFETIME(ABlasterGameState, BlueTeamScore);
	DOREPLIFETIME(ABlasterGameState, CurrentRound);
	DOREPLIFETIME(ABlasterGameState, MaxRounds);
}

void ABlasterGameState::UpdateTopScore(ABlasterPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	// 동점인 경우
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		// 배열에 중복 항목 추가 x
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		// 전부 비우고
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void ABlasterGameState::RedTeamScores()
{
	++RedTeamScore;

	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScores(RedTeamScore);
	}
}

void ABlasterGameState::BlueTeamScores()
{
	++BlueTeamScore;

	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScores(BlueTeamScore);
	}
}

void ABlasterGameState::OnRep_RedTeamScore()
{
	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScores(RedTeamScore);
	}
}

void ABlasterGameState::OnRep_BlueTeamScore()
{

	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScores(BlueTeamScore);
	}
}

void ABlasterGameState::OnRep_CurrentRound()
{
	// 클라에서 라운드가 변경 될 시 필요한 로직 처리
}


void ABlasterGameState::SetCurrentRound(int32 NewRound)
{
	CurrentRound = NewRound;
}

void ABlasterGameState::SetMaxRounds(int32 NewMaxRounds)
{
	MaxRounds = NewMaxRounds;
}

void ABlasterGameState::AddWeaponPurchase(EWeaponType WeaponType)
{
	if (!GamePurchaseStats.WeaponPurchases.Contains(WeaponType))
	{
		GamePurchaseStats.WeaponPurchases.Add(WeaponType, 1);
	}
	else
	{
		GamePurchaseStats.WeaponPurchases[WeaponType]++;
	}
}

void ABlasterGameState::AddBuffPurchase(EBuffType BuffType)
{
	if (!GamePurchaseStats.BuffPurchases.Contains(BuffType))
	{
		GamePurchaseStats.BuffPurchases.Add(BuffType, 1);
	}
	else
	{
		GamePurchaseStats.BuffPurchases[BuffType]++;
	}
}

void ABlasterGameState::AddThrowablePurchase(EThrowType ThrowType, int32 Count)
{
	if (!GamePurchaseStats.ThrowablePurchases.Contains(ThrowType))
	{
		GamePurchaseStats.ThrowablePurchases.Add(ThrowType, Count);
	}
	else
	{
		GamePurchaseStats.ThrowablePurchases[ThrowType] += Count;
	}
}

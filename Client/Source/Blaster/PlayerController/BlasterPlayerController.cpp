


#include "PlayerController/BlasterPlayerController.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Blaster/Character/MyBlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Components/Image.h"
#include "Blaster/HUD/ReturnToMainMenu.h"
#include "Blaster/BlasterTypes/Announcement.h"
#include "Blaster/HUD/Shop.h"
#include "Blaster/BlasterComponents/ShopComponent.h"
#include "Blaster/HUD/Board/ScoreBoard.h"
#include "Blaster/HUD/Board/TeamScoreBoard.h"
#include "GameInstance/BlasterGameInstance.h"


void ABlasterPlayerController::BroadcastElim(APlayerState* Attacker, APlayerState* Victim)
{
	ClientElimAnnouncement(Attacker, Victim);
}

void ABlasterPlayerController::ClientElimAnnouncement_Implementation(APlayerState* Attacker, APlayerState* Victim)
{
	APlayerState* Self = GetPlayerState<APlayerState>();
	if (Attacker && Victim && Self)
	{
		BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
		if (BlasterHUD)
		{
			if (Attacker == Self && Victim != Self)
			{
				BlasterHUD->AddElimAnnouncement("You", Victim->GetPlayerName());
				return;
			}
			if (Victim == Self && Attacker != Self)
			{
				BlasterHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "you");
				return;
			}
			if (Attacker == Victim && Attacker == Self)
			{
				BlasterHUD->AddElimAnnouncement("You", "yourself");
				return;
			}
			if (Attacker == Victim && Attacker != Self)
			{
				BlasterHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "themselves");
				return;
			}
			BlasterHUD->AddElimAnnouncement(Attacker->GetPlayerName(), Victim->GetPlayerName());
		}
	}

}

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	ServerCheckMatchState();

	SetInputMode(FInputModeGameOnly());
	SetupUIInputMode();

	if (UBlasterGameInstance* GI = Cast<UBlasterGameInstance>(GetGameInstance()))
	{
		GI->SetShowRoomFlag(true);
	}
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, MatchState);
	DOREPLIFETIME(ABlasterPlayerController, bShowTeamScores);
	DOREPLIFETIME(ABlasterPlayerController, StateStartTime);
}

void ABlasterPlayerController::HideTeamScores()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RedTeamScore &&
		BlasterHUD->CharacterOverlay->BlueTeamScore &&
		BlasterHUD->CharacterOverlay->ScoreSpacerText;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->RedTeamScore->SetText(FText());
		BlasterHUD->CharacterOverlay->BlueTeamScore->SetText(FText());
		BlasterHUD->CharacterOverlay->ScoreSpacerText->SetText(FText());
	}
}

// 여기 수정 필요, 라운드 개념이 들어가서....
void ABlasterPlayerController::InitTeamScores()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RedTeamScore &&
		BlasterHUD->CharacterOverlay->BlueTeamScore &&
		BlasterHUD->CharacterOverlay->ScoreSpacerText;
	if (bHUDValid)
	{
		FString Spacer("|");
		FString RedScoreText = FString::Printf(TEXT("%d"), CachedRedScore);
		FString BlueScoreText = FString::Printf(TEXT("%d"), CachedBlueScore);
		BlasterHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(RedScoreText));
		BlasterHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(BlueScoreText));
		BlasterHUD->CharacterOverlay->ScoreSpacerText->SetText(FText::FromString(Spacer));
	}
}

void ABlasterPlayerController::SetHUDRedTeamScores(int32 RedScore)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	FString ScoreText = FString::Printf(TEXT("%d"), RedScore);

	CachedRedScore = RedScore;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RedTeamScore;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(ScoreText));
	}
}

void ABlasterPlayerController::SetHUDBlueTeamScores(int32 BlueScore)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	FString ScoreText = FString::Printf(TEXT("%d"), BlueScore);

	CachedBlueScore = BlueScore;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->BlueTeamScore;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(ScoreText));
	}
}

void ABlasterPlayerController::SetHUDMoney(int32 NewMoney)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->Shop &&
		BlasterHUD->Shop->MoneyText;
	if (bHUDValid)
	{
		FString Money = FString::Printf(TEXT("%d"), NewMoney);
		BlasterHUD->Shop->MoneyText->SetText(FText::FromString(Money));
	}
}

void ABlasterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
	CheckPing(DeltaTime);
}

void ABlasterPlayerController::CheckPing(float DeltaTime)
{
	if (HasAuthority()) return;
	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime > CheckPingFrequency)
	{
		PlayerState = PlayerState == nullptr ? *GetPlayerState<APlayerState>() : PlayerState;
		if (PlayerState)
		{
			if (PlayerState->GetCompressedPing() * 4 > HighPingThreshold)
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.f;
				ServerReportPingStatus(true);
			}
			else
			{
				ServerReportPingStatus(false);
			}
		}
		HighPingRunningTime = 0.f;
	}
	bool bHighPingAnimationPlaying =
		BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingAnimation &&
		BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation);
	if (bHighPingAnimationPlaying)
	{
		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();
		}
	}
}

void ABlasterPlayerController::ShowReturnToMainMenu()
{
	// show the Return to Main Menu widget
	if (ReturnToMainMenuWidget == nullptr) return;
	if (ReturnToMainMenu == nullptr)
	{
		ReturnToMainMenu = CreateWidget<UReturnToMainMenu>(this, ReturnToMainMenuWidget); // 이 작업을 수행한 후, 주 메뉴로 돌아가는 것이 유효해야 함 (적어도 메뉴가 파괴될 때까진 유효해야 됨.)
	}
	if (ReturnToMainMenu)
	{
		bReturnToMainMenuOpen = !bReturnToMainMenuOpen;
		if (bReturnToMainMenuOpen)
		{
			ReturnToMainMenu->MenuSetup();
		}
		else
		{
			ReturnToMainMenu->MenuTearDown();
		}
	}

}

// 클라에도 알려주기 위해
void ABlasterPlayerController::OnRep_ShowTeamScores()
{
	if (bShowTeamScores)
	{
		InitTeamScores();
	}
	else
	{
		HideTeamScores();
	}
}

// Is the Ping too high?
void ABlasterPlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing);
}

void ABlasterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ABlasterPlayerController::HighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingImage &&
		BlasterHUD->CharacterOverlay->HighPingAnimation;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		BlasterHUD->CharacterOverlay->PlayAnimation(
			BlasterHUD->CharacterOverlay->HighPingAnimation,
			0.f,
			5);
	}
}

void ABlasterPlayerController::StopHighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingImage &&
		BlasterHUD->CharacterOverlay->HighPingAnimation;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(0.f);

		if (BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation))
		{
			BlasterHUD->CharacterOverlay->StopAnimation(BlasterHUD->CharacterOverlay->HighPingAnimation);
		}
	}
}




void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		bShowTeamScores = GameMode->bTeamsMatch;
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		StateStartTime = GameMode->StateStartTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, StateStartTime, bShowTeamScores);

	}
}

// 커뮤
void ABlasterPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime, bool bIsTeamsMatch)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	StateStartTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState, bIsTeamsMatch);

	// 진행중인 상태에 중도난입하면 공지 추가 x
	if (BlasterHUD && MatchState == MatchState::WaitingToStart)
	{
		BlasterHUD->AddAnnouncement();
	}
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AMyBlasterCharacter* BlasterCharacter = Cast<AMyBlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
		// 커뮤 - 죽고 태어났을 때 초기화 안되는 문제 해결법
		SetHUDShield(BlasterCharacter->GetShield(), BlasterCharacter->GetMaxShield());
		SetHUDGrenades(BlasterCharacter->GetCombat()->GetGrenades());
	}
}


void ABlasterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;

	InputComponent->BindAction("Quit", IE_Pressed, this, &ABlasterPlayerController::ShowReturnToMainMenu);
	InputComponent->BindAction("ScoreBoard", IE_Pressed, this, &ABlasterPlayerController::HandleShowScoreboard);
	InputComponent->BindAction("ScoreBoard", IE_Released, this, &ABlasterPlayerController::HandleHideScoreboard);
}

ABlasterPlayerController::ABlasterPlayerController()
{
	Shop = CreateDefaultSubobject<UShopComponent>(TEXT("ShopComponent"));
	Shop->SetIsReplicated(true);
}

void ABlasterPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Shop)
	{
		Shop->Controller = this;
		Shop->PlayerState = GetPlayerState<ABlasterPlayerState>();
	}
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HealthBar &&
		BlasterHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ABlasterPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ShieldBar &&
		BlasterHUD->CharacterOverlay->ShieldText;
	if (bHUDValid)
	{
		const float ShieldPercent = Shield / MaxShield;
		BlasterHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		BlasterHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
	}
	// 캐릭터 오버레이가 초기화되기 전에 너무 일찍 호출하려고 시도하는 경우에 대비해서 값 저장해둠.
	else
	{
		bInitializeShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ScoreAmount;

	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeScore = true;
		HUDScore = Score;
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->DefeatsAmount;

	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeDefeats = true;
		HUDDefeats = Defeats;
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	// 캐릭터 오버레이가 마지막에 초기화 되는 문제로 인해 게임 초반에 HUD에 대한 정확한 탄약량을 업데이트하지 못할 위험이 있다.
	else
	{
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
	}
}

void ABlasterPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->MatchCountdownText;

	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}


		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	// Visible 방식으로 필요한 것만 업데이트 하려고 했는데 오류 나서 일단 둠.

	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	// 카운트다운 텍스트 생성
	FString CountdownText;
	if (CountdownTime < 0.f)
	{
		CountdownText = FString();
	}
	else
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	}

	bool bAnnouncementValid = BlasterHUD &&
		BlasterHUD->Announcement &&
		BlasterHUD->Announcement->WarmupTime;

	if (bAnnouncementValid)
	{
		BlasterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}

	bool bShopValid = BlasterHUD &&
		BlasterHUD->Shop &&
		BlasterHUD->Shop->WarmupTime;

	if (bShopValid)
	{
		BlasterHUD->Shop->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDGrenades(int32 Grenades)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->GrenadesText;

	if (bHUDValid)
	{
		FString GrenadesText = FString::Printf(TEXT("%d"), Grenades);
		BlasterHUD->CharacterOverlay->GrenadesText->SetText(FText::FromString(GrenadesText));
	}
	// 캐릭터 오버레이가 가장 늦게 초기화 되므로.
	else
	{
		bInitializeGrenades = true;
		HUDGrenades = Grenades;
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	// 커뮤
	if (HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if (BlasterGameMode)
		{
			StateStartTime = BlasterGameMode->StateStartTime;
		}
	}

	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + StateStartTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = MatchTime - GetServerTime() + StateStartTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime - GetServerTime() + StateStartTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);


	// SecondsLeft는 어떻게 되든 상관없음. (타이밍만 맞추려고 쓰는 느낌.)
	if (HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if (BlasterGameMode)
		{
			SecondsLeft = FMath::CeilToInt(BlasterGameMode->GetCountdownTime() + StateStartTime);
		}
	}

	if (CountdownInt != SecondsLeft) // 1/2 지났다는 뜻
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}

	CountdownInt = SecondsLeft;
}

// CharacterOverlay 초기화가 느림 -> PollInit로 해결.
void ABlasterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				// 이렇게 초기화 여부를 다 따로 해줘야 일부만 초기화하는 버그를 해결 가능!
				if (bInitializeHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
				if (bInitializeShield) SetHUDShield(HUDShield, HUDMaxShield);
				if (bInitializeScore) SetHUDScore(HUDScore);
				if (bInitializeDefeats) SetHUDDefeats(HUDDefeats);
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);

				AMyBlasterCharacter* BlasterCharacter = Cast<AMyBlasterCharacter>(GetPawn());
				if (BlasterCharacter && BlasterCharacter->GetCombat())
				{
					if (bInitializeGrenades) SetHUDGrenades(BlasterCharacter->GetCombat()->GetGrenades());
				}
			}
		}
	}
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	// 서버의 역할 : 클라이언트의 요청 시간과 함께 자신의 현재 시간을 되돌려보냄.
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClentRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClentRequest;
	SingleTripTime = 0.5f * RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

// 서버에서만 일어남. (게임모드에서 호출하니깐)
// 클라도 매치 상태가 뭔지 알아야 됨.
// 상태가 바뀌면 RPC 발동하겠지!!
void ABlasterPlayerController::OnMatchStateSet(FName State, bool bTeamsMatch)
{
	MatchState = State;

	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		StateStartTime = GameMode->StateStartTime;
	}

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bTeamsMatch);
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}


void ABlasterPlayerController::HandleShowScoreboard()
{
	if (bShowTeamScores)  // 팀모드
	{
		if (!TeamScoreBoardWidget && TeamScoreBoardClass)
		{
			TeamScoreBoardWidget = CreateWidget<UTeamScoreBoard>(this, TeamScoreBoardClass);
		}
		if (TeamScoreBoardWidget)
		{
			TeamScoreBoardWidget->AddToViewport();
			UpdateTeamScoreboard();
		}
	}
	else  // 데스매치
	{
		if (!ScoreBoardWidget && ScoreBoardClass)
		{
			ScoreBoardWidget = CreateWidget<UScoreBoard>(this, ScoreBoardClass);
		}
		if (ScoreBoardWidget)
		{
			ScoreBoardWidget->AddToViewport();
			UpdateScoreboard();
		}
	}
}

void ABlasterPlayerController::HandleHideScoreboard()
{
	if (TeamScoreBoardWidget)
	{
		TeamScoreBoardWidget->RemoveFromParent();
	}
	if (ScoreBoardWidget)
	{
		ScoreBoardWidget->RemoveFromParent();
	}
}

void ABlasterPlayerController::UpdateScoreboard()
{
	TArray<FPlayerScoreData> ScoreDataArray;
	if (AGameStateBase* GameState = GetWorld()->GetGameState())
	{
		for (APlayerState* CurrentPlayer : GameState->PlayerArray)
		{
			if (ABlasterPlayerState* BlasterPS = Cast<ABlasterPlayerState>(CurrentPlayer))
			{
				FPlayerScoreData PlayerData;
				PlayerData.PlayerName = BlasterPS->GetPlayerName();
				PlayerData.Kill = BlasterPS->GetScore();
				PlayerData.Death = BlasterPS->GetDefeats();
				ScoreDataArray.Add(PlayerData);
			}
		}
	}
	if (ScoreBoardWidget)
	{
		ScoreBoardWidget->UpdateScoreboard(ScoreDataArray);
	}
}

void ABlasterPlayerController::UpdateTeamScoreboard()
{
	TArray<FTeamPlayerScoreData> ScoreDataArray;
	if (ABlasterGameState* BlasterGS = Cast<ABlasterGameState>(GetWorld()->GetGameState()))
	{
		for (APlayerState* CurrentPlayer : BlasterGS->PlayerArray)
		{
			if (ABlasterPlayerState* BlasterPS = Cast<ABlasterPlayerState>(CurrentPlayer))
			{
				FTeamPlayerScoreData PlayerData;
				PlayerData.PlayerName = BlasterPS->GetPlayerName();
				PlayerData.Kill = BlasterPS->GetScore();
				PlayerData.Death = BlasterPS->GetDefeats();
				PlayerData.Coin = BlasterPS->GetMoney();
				PlayerData.Team = BlasterPS->GetTeam();
				ScoreDataArray.Add(PlayerData);
			}
		}

		if (TeamScoreBoardWidget)
		{
			TeamScoreBoardWidget->UpdateTeamScoreboard(
				ScoreDataArray,
				BlasterGS->RedTeamScore,
				BlasterGS->BlueTeamScore
			);
		}
	}
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bShowTeamScores); // 이거 때문이었네...
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::ApplyCachedScores()
{
	if (BlasterHUD && BlasterHUD->Shop)
	{
		FString BlueScoreText = FString::Printf(TEXT("%d"), CachedBlueScore);
		FString RedScoreText = FString::Printf(TEXT("%d"), CachedRedScore);


		bool bHUDValid = BlasterHUD &&
			BlasterHUD->Shop &&
			BlasterHUD->Shop->RedTeamScore &&
			BlasterHUD->Shop->BlueTeamScore;

		if (bHUDValid)
		{
			BlasterHUD->Shop->BlueTeamScore->SetText(FText::FromString(BlueScoreText));
			BlasterHUD->Shop->RedTeamScore->SetText(FText::FromString(RedScoreText));
		}
	}
}

void ABlasterPlayerController::SetupUIInputMode()
{
	SetShowMouseCursor(false);
}


// good
void ABlasterPlayerController::HandleMatchHasStarted(bool bTeamsMatch)
{
	if (HasAuthority()) bShowTeamScores = bTeamsMatch;
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay == nullptr) BlasterHUD->AddCharacterOverlay();
		// 매치기능 추가로 인해 추가됨.
		else
		{
			BlasterHUD->CharacterOverlay->SetVisibility(ESlateVisibility::Visible);
		}
		if (BlasterHUD->Announcement)
		{
			// 승리나 패배 관련으로 Announcement을 재활용 할 것이므로 제거는 안함.
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
		if (BlasterHUD->Shop)
		{
			BlasterHUD->Shop->HideShop();
		}
		//if (!HasAuthority()) return;
		if (!IsLocalController()) return;
		// 안된 이유 : false -> false는 값이 바뀌지 않아 값복사가 되지 않음...
		if (bTeamsMatch)
		{
			InitTeamScores();
		}
		else
		{
			HideTeamScores();
		}
	}

	// 게임플레이 활성화
	if (AMyBlasterCharacter* BlasterCharacter = Cast<AMyBlasterCharacter>(GetPawn()))
	{
		BlasterCharacter->bDisableGameplay = false;
	}
}

void ABlasterPlayerController::HandleCooldown()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
		}

		// GameState 체크
		ABlasterGameState* BlasterGS = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
		if (!BlasterGS) return;

		// 캐릭터 게임플레이 비활성화
		AMyBlasterCharacter* BlasterCharacter = Cast<AMyBlasterCharacter>(GetPawn());
		if (BlasterCharacter && BlasterCharacter->GetCombat())
		{
			BlasterCharacter->bDisableGameplay = true;
			// 이렇게 해줘야 Cooldown 상태에 들어가자마자 거짓이 되고 캐릭터가 발사를 멈추게 됨.
			BlasterCharacter->GetCombat()->FireButtonPressed(false);

			// 스나이퍼 스코프 처리 추가
			if (BlasterCharacter->IsSniperAiming())
			{
				BlasterCharacter->ShowSniperScopeWidget(false);
			}
		}

		// 라운드 체크
		bool bIsMatchEnded = BlasterGS->GetCurrentRound() >= BlasterGS->GetMaxRounds();

		if (bIsMatchEnded)
		{
			bool bHUDValid = BlasterHUD &&
				BlasterHUD->Announcement &&
				BlasterHUD->Announcement->AnnouncementText &&
				BlasterHUD->Announcement->InfoText;

			if (bHUDValid)
			{
				BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
				BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(TEXT("Game End!")));

				ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
				ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
				if (BlasterGameState && BlasterPlayerState)
				{
					TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
					FString InfoTextString = bShowTeamScores ? GetTeamsInfoText(BlasterGameState) : GetInfoText(TopPlayers);
					BlasterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
				}

				// 추가 코드
				FTimerHandle ReturnTimer;
				GetWorldTimerManager().SetTimer(
					ReturnTimer,
					this,
					&ABlasterPlayerController::ReturnToMainMenuAfterMatch,
					CooldownTime,
					false
				);
			}	
		}
		else
		{
			if (!BlasterHUD->Shop)
			{
				BlasterHUD->AddShop();
			}
			BlasterHUD->Shop->ShowShop();

			// 일부러 유저들 숫자 바뀌는 것 보라고 딜레이를 줌.
			FTimerHandle ScoreUpdateTimer;
			GetWorldTimerManager().SetTimer(
				ScoreUpdateTimer,
				this,
				&ABlasterPlayerController::ApplyCachedScores,
				0.5f,
				false
			);
		}

	}
}

void ABlasterPlayerController::ReturnToMainMenuAfterMatch()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (HasAuthority()) // 서버
	{
		World->GetFirstPlayerController()->ClientTravel("/Game/Maps/GameStartupMap", TRAVEL_Absolute);
		World->ServerTravel("/Game/Maps/GameStartupMap");
	}
	else // 클라이언트
	{
		ClientTravel("/Game/Maps/GameStartupMap", TRAVEL_Absolute);
	}
}

FString ABlasterPlayerController::GetInfoText(const TArray<class ABlasterPlayerState*>& Players)
{
	ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
	if (BlasterPlayerState == nullptr) return FString();
	FString InfoTextString;
	if (Players.Num() == 0)
	{
		InfoTextString = Announcement::ThereIsNoWinner;
	}
	else if (Players.Num() == 1 && Players[0] == BlasterPlayerState)
	{
		InfoTextString = Announcement::YouAreTheWinner;
	}
	else if (Players.Num() == 1)
	{
		InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *Players[0]->GetPlayerName());
	}
	else if (Players.Num() > 1)
	{
		InfoTextString = Announcement::PlayersTiedForTheWin;
		InfoTextString.Append(FString("\n"));
		for (auto TiedPlayer : Players)
		{
			InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
		}
	}

	return InfoTextString;
}

FString ABlasterPlayerController::GetTeamsInfoText(ABlasterGameState* BlasterGameState)
{
	if (BlasterGameState == nullptr) return FString();
	FString InfoTextString;

	const int32 RedTeamScore = BlasterGameState->RedTeamScore;
	const int32 BlueTeamScore = BlasterGameState->BlueTeamScore;

	if (RedTeamScore == 0 && BlueTeamScore == 0)
	{
		InfoTextString = Announcement::ThereIsNoWinner;
	}
	else if (RedTeamScore == BlueTeamScore)
	{
		InfoTextString = FString::Printf(TEXT("%s\n"), *Announcement::TeamsTiedForTheWin);
		InfoTextString.Append(Announcement::RedTeam);
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(Announcement::BlueTeam);
		InfoTextString.Append(TEXT("\n"));
	}
	else if (RedTeamScore > BlueTeamScore)
	{
		InfoTextString = Announcement::RedTeamWins;
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::RedTeam, RedTeamScore));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::BlueTeam, BlueTeamScore));
	}
	else if (BlueTeamScore > RedTeamScore)
	{
		InfoTextString = Announcement::BlueTeamWins;
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::BlueTeam, BlueTeamScore));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::RedTeam, RedTeamScore));
	}

	return InfoTextString;
}

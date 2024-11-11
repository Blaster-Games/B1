


#include "PlayerState/BlasterPlayerState.h"
#include "Blaster/Character/MyBlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/BlasterTypes/ThrowTypes.h"
#include "Blaster/HUD/Shop.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);
	DOREPLIFETIME(ABlasterPlayerState, Team);
	DOREPLIFETIME(ABlasterPlayerState, Money);
	DOREPLIFETIME(ABlasterPlayerState, PurchasedWeapons);
	DOREPLIFETIME(ABlasterPlayerState, ActiveBuffs);
	DOREPLIFETIME(ABlasterPlayerState, ThrowableCounts);
	DOREPLIFETIME(ABlasterPlayerState, WeaponSlots);

}

void ABlasterPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// 권총 기본 지급
		AddWeapon(EWeaponType::EWT_Pistol);
		SetWeaponToSlot(EWeaponType::EWT_Pistol, 1);
	}

}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<AMyBlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	}
	if (Controller)
	{
		Controller->SetHUDScore(GetScore());
	}
}



void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<AMyBlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	}
	// 컨트롤러를 통해 HUD 접근 가능
	if (Controller)
	{
		Controller->SetHUDScore(GetScore()); // 복제된 값 사용.
	}
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<AMyBlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	}
	if (Controller)
	{
		Controller->SetHUDDefeats(Defeats);
	}
}

void ABlasterPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<AMyBlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	}
	if (Controller)
	{
		Controller->SetHUDDefeats(Defeats);
	}
}

void ABlasterPlayerState::SetMoney(int32 NewMoney)
{
	Money = NewMoney;

	Character = Character == nullptr ? Cast<AMyBlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	}
	if (Controller)
	{
		Controller->SetHUDMoney(NewMoney);
	}
}

void ABlasterPlayerState::OnRep_Money()
{
	Character = Character == nullptr ? Cast<AMyBlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	}
	if (Controller)
	{
		Controller->SetHUDMoney(Money);
	}
}



bool ABlasterPlayerState::HasWeapon(EWeaponType WeaponType) const
{
	return PurchasedWeapons.Contains(WeaponType);
}

void ABlasterPlayerState::AddWeapon(EWeaponType WeaponType)
{
	if (HasAuthority())
	{
		if (!PurchasedWeapons.Contains(WeaponType))
		{
			PurchasedWeapons.Add(WeaponType);
			OnWeaponPurchased.Broadcast(WeaponType);  // 무기 구매 시 알림
		}
	}
	
}

void ABlasterPlayerState::OnRep_PurchasedWeapons()
{
	// 마지막으로 추가된 무기에 대해 델리게이트 호출
	if (!PurchasedWeapons.IsEmpty())
	{
		OnWeaponPurchased.Broadcast(PurchasedWeapons.Last());
	}
}



void ABlasterPlayerState::AddBuff(EBuffType BuffType)
{
	if (!ActiveBuffs.Contains(BuffType))
	{
		ActiveBuffs.Add(BuffType);
		OnBuffStateChanged.Broadcast(BuffType, true);
	}
}

void ABlasterPlayerState::OnRep_ActiveBuffs()
{
	if (ActiveBuffs.IsEmpty())
	{
		// 모든 버프 타입에 대해 비활성화 상태 알림
		for (uint8 i = 0; i < static_cast<uint8>(EBuffType::EBT_MAX); ++i)
		{
			OnBuffStateChanged.Broadcast(static_cast<EBuffType>(i), false);
		}
	}
	else
	{
		OnBuffStateChanged.Broadcast(ActiveBuffs.Last(), true);
	}
}

bool ABlasterPlayerState::HasBuff(EBuffType BuffType) const
{
	return ActiveBuffs.Contains(BuffType);
}

void ABlasterPlayerState::ClearBuff()
{
	if (HasAuthority())
	{
		// 초기화 전에 현재 가지고 있는 모든 버프에 대해 비활성화 알림
		for (EBuffType Buff : ActiveBuffs)
		{
			OnBuffStateChanged.Broadcast(Buff, false);
		}
		ActiveBuffs.Empty();
	}
}
void ABlasterPlayerState::OnRep_ThrowableCounts()
{
}


int32 ABlasterPlayerState::GetThrowableCount(EThrowType ThrowType) const
{
	for (const FThrowableInfo& ThrowInfo : ThrowableCounts)
	{
		if (ThrowInfo.ThrowType == ThrowType)
		{
			return ThrowInfo.Count;
		}
	}
	return 0; // 해당 타입이 없으면 0 반환
}

void ABlasterPlayerState::SetThrowableCount(EThrowType ThrowType, int32 Count)
{
	bool bFound = false;
	// 기존 항목이 있는지 찾기
	for (FThrowableInfo& ThrowInfo : ThrowableCounts)
	{
		if (ThrowInfo.ThrowType == ThrowType)
		{
			ThrowInfo.Count = Count;
			bFound = true;
			break;
		}
	}

	// 없으면 새로 추가
	if (!bFound)
	{
		FThrowableInfo NewInfo;
		NewInfo.ThrowType = ThrowType;
		NewInfo.Count = Count;
		ThrowableCounts.Add(NewInfo);
	}

}

void ABlasterPlayerState::AddToThrowableCount(EThrowType ThrowType, int32 AddCount)
{
	int32 CurrentCount = GetThrowableCount(ThrowType);
	SetThrowableCount(ThrowType, CurrentCount + AddCount);
}

void ABlasterPlayerState::SetWeaponToSlot(EWeaponType WeaponType, int32 SlotIndex)
{
	if (HasAuthority())
	{
		// 내가 이미 구매를 했던 무기라면 슬롯 장착.
		if (HasWeapon(WeaponType))
		{
			if (SlotIndex == 0) WeaponSlots.Slot1Weapon = WeaponType;
			else if (SlotIndex == 1) WeaponSlots.Slot2Weapon = WeaponType;

			OnWeaponSlotsUpdated.Broadcast();

		}
	}
}

void ABlasterPlayerState::OnRep_WeaponSlots()
{
	OnWeaponSlotsUpdated.Broadcast();
}

EWeaponType ABlasterPlayerState::GetWeaponInSlot(int32 SlotIndex) const
{
	return SlotIndex == 0 ? WeaponSlots.Slot1Weapon : WeaponSlots.Slot2Weapon;
}

bool ABlasterPlayerState::IsSlotEmpty(int32 SlotIndex) const
{
	EWeaponType WeaponInSlot = GetWeaponInSlot(SlotIndex);
	return WeaponInSlot == EWeaponType::EWT_MAX;
}

void ABlasterPlayerState::ClearWeaponSlots()
{
	if (HasAuthority())
	{
		WeaponSlots.Slot1Weapon = EWeaponType::EWT_MAX;
		WeaponSlots.Slot2Weapon = EWeaponType::EWT_MAX;
	}
}


void ABlasterPlayerState::SetTeam(ETeam TeamToSet)
{
	Team = TeamToSet;

	AMyBlasterCharacter* BCharacter = Cast<AMyBlasterCharacter>(GetPawn());
	if (BCharacter)
	{
		BCharacter->SetTeamColor(Team);
	}
}




void ABlasterPlayerState::OnRep_Team()
{
	AMyBlasterCharacter* BCharacter = Cast<AMyBlasterCharacter>(GetPawn());
	if (BCharacter)
	{
		BCharacter->SetTeamColor(Team);
	}
}


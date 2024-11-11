

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Blaster/BlasterTypes/Team.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/BuffTypes.h"
#include "Blaster/BlasterTypes/ThrowTypes.h"
#include "BlasterPlayerState.generated.h"

// 델리게이트도 한번 써보고 싶어서 써봄.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponPurchasedDelegate, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponSlotsUpdated);

USTRUCT()
struct FThrowableInfo
{
	GENERATED_BODY()

	UPROPERTY()
	EThrowType ThrowType;

	UPROPERTY()
	int32 Count;
};

USTRUCT(BlueprintType)
struct FWeaponSlots
{
	GENERATED_BODY()

	UPROPERTY()
	EWeaponType Slot1Weapon = EWeaponType::EWT_MAX;

	UPROPERTY()
	EWeaponType Slot2Weapon = EWeaponType::EWT_MAX;
};

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	// 델리게이트 관련
	FOnWeaponPurchasedDelegate OnWeaponPurchased;
	FOnWeaponSlotsUpdated OnWeaponSlotsUpdated;

	/**
	* Replication notifies
	*/
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();
	
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

	// 무기
	bool HasWeapon(EWeaponType WeaponType) const;
	void AddWeapon(EWeaponType WeaponType);

	// 버프
	void AddBuff(EBuffType BuffType);
	bool HasBuff(EBuffType BuffType) const;
	void ClearBuff();

	// 투척
	int32 GetThrowableCount(EThrowType ThrowType) const;
	void SetThrowableCount(EThrowType ThrowType, int32 Count);
	void AddToThrowableCount(EThrowType ThrowType, int32 AddCount);

	// 무기 슬롯
	void SetWeaponToSlot(EWeaponType WeaponType, int32 SlotIndex);
	EWeaponType GetWeaponInSlot(int32 SlotIndex) const;
	bool IsSlotEmpty(int32 SlotIndex) const;
	void ClearWeaponSlots();
	FWeaponSlots GetWeaponSlots() const { return WeaponSlots; }

private:
	// 처음에 nullptr로 밀어주기 위해 = nullptr;로 해줘도 되고 언리얼 엔진에서는 새로운 속성을 주면 됨 (UPROPERTY())
	UPROPERTY()
	class AMyBlasterCharacter* Character;
	UPROPERTY()
	class ABlasterPlayerController* Controller;

	// 점수(Score)는 기본 제공.

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
	
	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam Team = ETeam::ET_NoTeam;

	UFUNCTION()
	void OnRep_Team();

	/**
	*  Shop
	*/

	// 소유한 돈
	UPROPERTY(ReplicatedUsing = OnRep_Money)
	int32 Money = 1000;

	UFUNCTION()
	void OnRep_Money();



	// 구매한 무기 목록
	UPROPERTY(ReplicatedUsing = OnRep_PurchasedWeapons)
	TArray<EWeaponType> PurchasedWeapons;

	UFUNCTION()
	void OnRep_PurchasedWeapons();

	// 한 라운드 동안 활성화될 버프 목록
	UPROPERTY(ReplicatedUsing = OnRep_ActiveBuffs)
	TArray<EBuffType> ActiveBuffs;

	UFUNCTION()
	void OnRep_ActiveBuffs();
	
	// 투척 무기 목록
	UPROPERTY(ReplicatedUsing = OnRep_ThrowableCounts)
	TArray<FThrowableInfo> ThrowableCounts;

	UFUNCTION()
	void OnRep_ThrowableCounts();

	// 무기 슬롯
	UPROPERTY(ReplicatedUsing = OnRep_WeaponSlots)
	FWeaponSlots WeaponSlots;

	UFUNCTION()
	void OnRep_WeaponSlots();



	// 어떤 유저가 눌렀는지 아는 방법은 ReturnToMainMenu 참고하자

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam TeamToSet);
	FORCEINLINE int32 GetMoney() const { return Money; }
	void SetMoney(int32 NewMoney);


};

// 만약에 위의 정보들을 여기서 관리하지 않고 캐릭터로 관리한다면?
// -> 리스폰 시 데이터 손실 문제가 있음.


// 플레이어 컨트롤러는 playerstate에 접속하는 기능을 기본 제공.
// 하지만 playerstate는 플레이어 컨트롤러에 접속하는 직접적인 기능이 없음.
// 플레이어 상태와 관련된 pawn에는 엑세스 가능하다

// PlayerController가 주로 클라이언트 측 논리를 처리하는 반면, PlayerState는 서버와 클라이언트 모두에서 플레이어의 상태 정보를 추적하는 데 사용되기 때문에 의도된 설계



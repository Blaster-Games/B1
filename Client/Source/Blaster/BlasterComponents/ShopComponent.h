

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/BlasterTypes/ShopStructs.h"
#include "ShopComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UShopComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UShopComponent();
	friend class ABlasterPlayerController;
	// 이건 프레임 단위로 할 때 쓰는거라 불필요하다고 느끼면 지워버리기!
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RequestWeaponPurchase(const FWeaponData& WeaponData);
	void RequestThrowablePurchase(const FThrowData& ThrowData);
	void RequestBuffPurchase(const FBuffData& BuffData);

	void RequestWeaponSlot1(const FWeaponData& WeaponData);
	void RequestWeaponSlot2(const FWeaponData& WeaponData);


	// 무기 슬롯에 해당 무기 장착 요청을 하면 서버에서 실제로 해당 무기를 해당 유저가 구매를 했는지 파악하고
	// 적용

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerWeaponPurchase(const FWeaponData& WeaponData);

	UFUNCTION(Server, Reliable)
	void ServerThrowablePurchase(const FThrowData& ThrowData);

	UFUNCTION(Server, Reliable)
	void ServerBuffPurchase(const FBuffData& BuffData);

	UFUNCTION(Server, Reliable)
	void ServerWeaponSlot1(const FWeaponData& WeaponData);

	UFUNCTION(Server, Reliable)
	void ServerWeaponSlot2(const FWeaponData& WeaponData);

	// 할지말지 고민중.
	// 클라이언트에 결과 알림
	/*UFUNCTION(Client, Reliable)
	void ClientPurchaseResult(bool bSuccess, const FString& Message);*/
	

private:	
	UPROPERTY()
	class ABlasterPlayerController* Controller;
		
	UPROPERTY()
	class ABlasterPlayerState* PlayerState;

	UPROPERTY()
	class ABlasterGameState* BlasterGameState;

public:
	// Character는 필요할 때마다 GetPawn()으로 가져오도록 변경
	class AMyBlasterCharacter* GetCharacter() const;
	FORCEINLINE ABlasterPlayerState* GetPlayerState() const { return PlayerState; }
};

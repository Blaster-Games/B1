


#include "ShopComponent.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/Character/MyBlasterCharacter.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

UShopComponent::UShopComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UShopComponent::BeginPlay()
{
	Super::BeginPlay();

    Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(GetOwner()) : Controller;
    if (Controller)
    {
        PlayerState = PlayerState == nullptr ? Cast<ABlasterPlayerState>(Controller->PlayerState) : PlayerState;
    }
}


void UShopComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UShopComponent::RequestWeaponPurchase(const FWeaponData& WeaponData)
{
    ServerWeaponPurchase(WeaponData);
}

void UShopComponent::RequestThrowablePurchase(const FThrowData& ThrowData)
{
    ServerThrowablePurchase(ThrowData);
}

void UShopComponent::RequestBuffPurchase(const FBuffData& BuffData)
{
    ServerBuffPurchase(BuffData);
}

void UShopComponent::RequestWeaponSlot1(const FWeaponData& WeaponData)
{
    ServerWeaponSlot1(WeaponData);
}

void UShopComponent::RequestWeaponSlot2(const FWeaponData& WeaponData)
{
    ServerWeaponSlot2(WeaponData);
}

void UShopComponent::ServerWeaponPurchase_Implementation(const FWeaponData& WeaponData)
{
    if (Controller)
    {
        PlayerState = PlayerState == nullptr ? Cast<ABlasterPlayerState>(Controller->PlayerState) : PlayerState;
    }

    if (!PlayerState) return;

    // 이미 가지고 있는 무기인지 확인
    if (PlayerState->HasWeapon(WeaponData.WeaponType))
    {
        return;
    }

    // 돈이 충분한지 확인
    if (PlayerState->GetMoney() >= WeaponData.Price)
    {
        // 돈 차감
        PlayerState->SetMoney(PlayerState->GetMoney() - WeaponData.Price);
        // 무기 추가
        PlayerState->AddWeapon(WeaponData.WeaponType);
    }
   
}

void UShopComponent::ServerWeaponSlot1_Implementation(const FWeaponData& WeaponData)
{
    if (Controller)
    {
        PlayerState = PlayerState == nullptr ? Cast<ABlasterPlayerState>(Controller->PlayerState) : PlayerState;
    }

    if (PlayerState)
    {
        PlayerState->SetWeaponToSlot(WeaponData.WeaponType, 0);
    }
}

void UShopComponent::ServerWeaponSlot2_Implementation(const FWeaponData& WeaponData)
{
    if (Controller)
    {
        PlayerState = PlayerState == nullptr ? Cast<ABlasterPlayerState>(Controller->PlayerState) : PlayerState;
    }

    if (PlayerState)
    {
        PlayerState->SetWeaponToSlot(WeaponData.WeaponType, 1);
    }
}

void UShopComponent::ServerThrowablePurchase_Implementation(const FThrowData& ThrowData)
{
    if (Controller)
    {
        PlayerState = PlayerState == nullptr ? Cast<ABlasterPlayerState>(Controller->PlayerState) : PlayerState;
    }

    if (!PlayerState) return;

    // 돈이 충분한지 확인
    if (PlayerState->GetMoney() >= ThrowData.Price)
    {
        // 돈 차감
        PlayerState->SetMoney(PlayerState->GetMoney() - ThrowData.Price);
        // 수류탄 추가
        PlayerState->AddToThrowableCount(ThrowData.ThrowType, 1);
    }
   
}

void UShopComponent::ServerBuffPurchase_Implementation(const FBuffData& BuffData)
{

    if (Controller)
    {
        PlayerState = PlayerState == nullptr ? Cast<ABlasterPlayerState>(Controller->PlayerState) : PlayerState;
    }

    if (!PlayerState) return;

    // 이미 해당 버프를 가지고 있는지 확인
    if (PlayerState->HasBuff(BuffData.BuffType))
    {
        return;
    }

    // 돈이 충분한지 확인
    if (PlayerState->GetMoney() >= BuffData.Price)
    {
        // 돈 차감
        PlayerState->SetMoney(PlayerState->GetMoney() - BuffData.Price);
        // 버프 추가
        PlayerState->AddBuff(BuffData.BuffType);
    }
}


AMyBlasterCharacter* UShopComponent::GetCharacter() const
{
    return Controller ? Cast<AMyBlasterCharacter>(Controller->GetPawn()) : nullptr;
}

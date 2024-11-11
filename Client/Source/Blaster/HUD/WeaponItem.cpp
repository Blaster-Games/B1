


#include "WeaponItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Blaster/Character/MyBlasterCharacter.h"
#include "Blaster/BlasterComponents/ShopComponent.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

void UWeaponItem::NativeConstruct()
{
    Super::NativeConstruct();

    if (BuyButton) BuyButton->OnClicked.AddDynamic(this, &UWeaponItem::OnBuyClicked);
    if (Slot1Button) Slot1Button->OnClicked.AddDynamic(this, &UWeaponItem::OnSlot1Clicked);
    if (Slot2Button) Slot2Button->OnClicked.AddDynamic(this, &UWeaponItem::OnSlot2Clicked);

    // 초기 상태 설정
    if (PriceInfo) PriceInfo->SetVisibility(ESlateVisibility::Visible);
    if (BuyButton) BuyButton->SetVisibility(ESlateVisibility::Visible);
    
    if (Slot1Button) Slot1Button->SetVisibility(ESlateVisibility::Collapsed);
    if (Slot2Button) Slot2Button->SetVisibility(ESlateVisibility::Collapsed);

    // ShopComponent 참조 얻기
    if (AMyBlasterCharacter* Character = Cast<AMyBlasterCharacter>(GetOwningPlayerPawn()))
    {
        ShopComponent = Character->GetShop();
    }

    if (AMyBlasterCharacter* Character = Cast<AMyBlasterCharacter>(GetOwningPlayerPawn()))
    {
        if (ABlasterPlayerState* PS = Cast<ABlasterPlayerState>(Character->GetPlayerState()))
        {
            PS->OnWeaponPurchased.AddDynamic(this, &UWeaponItem::OnWeaponPurchaseStateChanged);

            // 초기 상태 설정
            bool bIsPurchased = PS->HasWeapon(WeaponData.WeaponType);
            UpdatePurchaseState(bIsPurchased);
        }
    }
}

void UWeaponItem::SetWeaponData(const FWeaponData& InWeaponData)
{
    WeaponData = InWeaponData;

    // UI 업데이트
    if (WeaponNameText)
    {
        WeaponNameText->SetText(FText::FromString(WeaponData.WeaponName));
    }

    if (PriceText)
    {
        PriceText->SetText(FText::FromString(FString::Printf(TEXT("%d"), WeaponData.Price)));
    }

    if (WeaponImage)
    {
        // 이미지 로드 및 설정
        if (UTexture2D* Texture = WeaponData.WeaponImage.LoadSynchronous())
        {
            WeaponImage->SetBrushFromTexture(Texture);
        }
    }
    // 이미 구매한 무기인지 확인하고 UI 상태 업데이트
    if (ShopComponent && ShopComponent->GetPlayerState())
    {
        bool bIsPurchased = ShopComponent->GetPlayerState()->HasWeapon(WeaponData.WeaponType);
        UpdatePurchaseState(bIsPurchased);
    }
}

void UWeaponItem::UpdatePurchaseState(bool bIsPurchased)
{
    // 구매하지 않은 상태면 구매 관련 UI 표시
    if (PriceInfo)
    {
        PriceInfo->SetVisibility(bIsPurchased ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
    }
    if (BuyButton)
    {
        BuyButton->SetVisibility(bIsPurchased ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
    }

    // 구매한 상태면 슬롯 버튼들 표시
    if (Slot1Button)
    {
        Slot1Button->SetVisibility(bIsPurchased ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
    if (Slot2Button)
    {
        Slot2Button->SetVisibility(bIsPurchased ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UWeaponItem::OnBuyClicked()
{
    if (ShopComponent)
    {
        ShopComponent->RequestWeaponPurchase(WeaponData);
    }
}

void UWeaponItem::OnSlot1Clicked()
{
    if (ShopComponent)
    {
        ShopComponent->RequestWeaponSlot1(WeaponData);
    }
}

void UWeaponItem::OnSlot2Clicked()
{
    if (ShopComponent)
    {
        ShopComponent->RequestWeaponSlot2(WeaponData);
    }
}

void UWeaponItem::OnWeaponPurchaseStateChanged(EWeaponType PurchasedWeaponType)
{
    if (WeaponData.WeaponType == PurchasedWeaponType)
    {
        UpdatePurchaseState(true);  // UI 갱신
    }
}


// 돈처럼 단순한 수치 업데이트는 Controller를 통하는 게 좋지만, 
// 이런 복잡한 UI 상태 변경은 현재처럼 위젯 자체에서 처리하는 게 더 적절하다고 판단.



#include "HUD/AdditionalItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Blaster/Character/MyBlasterCharacter.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/BlasterComponents/ShopComponent.h"

void UAdditionalItem::NativeConstruct()
{
    Super::NativeConstruct();

    if (OwnedText) OwnedText->SetVisibility(ESlateVisibility::Collapsed);

    if (BuyButton) BuyButton->OnClicked.AddDynamic(this, &UAdditionalItem::OnBuyClicked);

    // ShopComponent 참조 얻기
    if (AMyBlasterCharacter* Character = Cast<AMyBlasterCharacter>(GetOwningPlayerPawn()))
    {
        ShopComponent = Character->GetShop();
    }


    // PlayerState의 델리게이트에 바인딩
    if (AMyBlasterCharacter* Character = Cast<AMyBlasterCharacter>(GetOwningPlayerPawn()))
    {
        if (ABlasterPlayerState* PS = Cast<ABlasterPlayerState>(Character->GetPlayerState()))
        {
            PS->OnBuffStateChanged.AddDynamic(this, &UAdditionalItem::OnBuffStateChanged);
        }
    }
}

void UAdditionalItem::SetThrowData(const FThrowData& InThrowData)
{
	ThrowData = InThrowData;
    bIsThrowable = true;

    // UI 업데이트
    if (ItemNameText)
    {
        ItemNameText->SetText(FText::FromString(ThrowData.ThrowName));
    }

    if (PriceText)
    {
        PriceText->SetText(FText::FromString(FString::Printf(TEXT("%d"), ThrowData.Price)));
    }

    if (ItemImage)
    {
        // 이미지 로드 및 설정
        if (UTexture2D* Texture = ThrowData.ThrowImage.LoadSynchronous())
        {
            ItemImage->SetBrushFromTexture(Texture);
        }
    }
}

void UAdditionalItem::SetBuffData(const FBuffData& InBuffData)
{
    BuffData = InBuffData;
    bIsThrowable = false;

    // UI 업데이트
    if (ItemNameText)
    {
        ItemNameText->SetText(FText::FromString(BuffData.BuffName));
    }

    if (PriceText)
    {
        PriceText->SetText(FText::FromString(FString::Printf(TEXT("%d"), BuffData.Price)));
    }

    if (ItemImage)
    {
        // 이미지 로드 및 설정
        if (UTexture2D* Texture = BuffData.BuffImage.LoadSynchronous())
        {
            ItemImage->SetBrushFromTexture(Texture);
        }
    }
}

void UAdditionalItem::OnBuyClicked()
{
    if (ShopComponent)
    {
        if (bIsThrowable)
        {
            ShopComponent->RequestThrowablePurchase(ThrowData);
        }
        else
        {
            ShopComponent->RequestBuffPurchase(BuffData);
        }
    }
}

void UAdditionalItem::OnBuffStateChanged(EBuffType BuffType, bool bActive)
{
    if (!bIsThrowable && BuffData.BuffType == BuffType)
    {
        UpdatePurchaseState(bActive);
    }
}



void UAdditionalItem::UpdatePurchaseState(bool bIsPurchased)
{
    // Owned 텍스트 처리
    if (OwnedText)
    {
        OwnedText->SetVisibility(bIsPurchased ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    // 구매 버튼과 가격 정보 처리
    if (BuyButton)
    {
        BuyButton->SetVisibility(bIsPurchased ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
    }
    if (PriceText)
    {
        PriceText->SetVisibility(bIsPurchased ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
    }
}




#include "HUD/AdditionalItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Blaster/Character/MyBlasterCharacter.h"
#include "Blaster/BlasterComponents/ShopComponent.h"

void UAdditionalItem::NativeConstruct()
{
    Super::NativeConstruct();

    if (BuyButton) BuyButton->OnClicked.AddDynamic(this, &UAdditionalItem::OnBuyClicked);

    // ShopComponent 참조 얻기
    if (AMyBlasterCharacter* Character = Cast<AMyBlasterCharacter>(GetOwningPlayerPawn()))
    {
        ShopComponent = Character->GetShop();
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

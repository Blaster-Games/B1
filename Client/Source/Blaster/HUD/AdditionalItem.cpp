


#include "HUD/AdditionalItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UAdditionalItem::SetThrowData(const FThrowData& InThrowData)
{
	ThrowData = InThrowData;

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

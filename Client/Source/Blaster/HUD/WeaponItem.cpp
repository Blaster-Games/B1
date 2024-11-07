


#include "WeaponItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

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
}

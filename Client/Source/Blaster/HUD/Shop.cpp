


#include "HUD/Shop.h"
#include "WeaponItem.h"
#include "AdditionalItem.h"
#include "Engine/DataTable.h"
#include "Blaster/BlasterTypes/ShopStructs.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Blaster/Character/MyBlasterCharacter.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

// 위젯이 생성되고 처음으로 초기화될 때 호출되는 함수
void UShop::NativeConstruct()
{
	Super::NativeConstruct();
	CreateWeaponItems();
    CreateThrowItems();
    CreateBuffItems();

    if (AMyBlasterCharacter* Character = Cast<AMyBlasterCharacter>(GetOwningPlayerPawn()))
    {
        if (ABlasterPlayerState* PS = Cast<ABlasterPlayerState>(Character->GetPlayerState()))
        {

            PlayerState = PS;
            PS->OnWeaponSlotsUpdated.AddDynamic(this, &UShop::UpdateSlotImages);
            UpdateMoneyText(PS->GetMoney());
            UpdateSlotImages();
        }
    }
}

void UShop::CreateWeaponItems()
{
    if (!ShopWeaponTable || !WeaponItemWidgetClass || !WeaponGrid)
        return;

    // DataTable의 모든 행을 순회
    for (auto It : ShopWeaponTable->GetRowMap())
    {
        FWeaponData* WeaponData = (FWeaponData*)It.Value;
        if (!WeaponData)
            continue;

        // WBP_WeaponItem 위젯 생성
        UWeaponItem* WeaponItemWidget = CreateWidget<UWeaponItem>(this, WeaponItemWidgetClass);
        if (WeaponItemWidget)
        {
            // 생성된 위젯에 데이터 설정
            WeaponItemWidget->SetWeaponData(*WeaponData);

            // Grid에 추가
            int32 Row = WeaponGrid->GetChildrenCount() / 2;
            int32 Column = WeaponGrid->GetChildrenCount() % 2;
            WeaponGrid->AddChildToUniformGrid(WeaponItemWidget, Row, Column);
        }
    }
}

void UShop::CreateThrowItems()
{
    if (!ShopThrowTable || !AdditionalItemWidgetClass || !ThrowGrid)
        return;

    // DataTable의 모든 행을 순회
    for (auto It : ShopThrowTable->GetRowMap())
    {
        FThrowData* ThrowData = (FThrowData*)It.Value;
        if (!ThrowData)
            continue;

        UAdditionalItem* AdditionalItemWidget = CreateWidget<UAdditionalItem>(this, AdditionalItemWidgetClass);
        if (AdditionalItemWidget)
        {
            // 생성된 위젯에 데이터 설정
            AdditionalItemWidget->SetThrowData(*ThrowData);

            // Grid에 추가
            int32 Row = ThrowGrid->GetChildrenCount() / 2;
            int32 Column = ThrowGrid->GetChildrenCount() % 2;
            ThrowGrid->AddChildToUniformGrid(AdditionalItemWidget, Row, Column);
        }
    }
}

void UShop::CreateBuffItems()
{
    if (!ShopBuffTable || !AdditionalItemWidgetClass || !BuffGrid)
        return;

    // DataTable의 모든 행을 순회
    for (auto It : ShopBuffTable->GetRowMap())
    {
        FBuffData* BuffData = (FBuffData*)It.Value;
        if (!BuffData)
            continue;

        UAdditionalItem* AdditionalItemWidget = CreateWidget<UAdditionalItem>(this, AdditionalItemWidgetClass);
        if (AdditionalItemWidget)
        {
            // 생성된 위젯에 데이터 설정
            AdditionalItemWidget->SetBuffData(*BuffData);

            // Grid에 추가
            int32 Row = BuffGrid->GetChildrenCount() / 2;
            int32 Column = BuffGrid->GetChildrenCount() % 2;
            BuffGrid->AddChildToUniformGrid(AdditionalItemWidget, Row, Column);
        }
    }
}

void UShop::UpdateMoneyText(int32 NewMoney)
{
    if (MoneyText)
    {
        MoneyText->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewMoney)));
    }
}

void UShop::UpdateSlotImages()
{
    if (AMyBlasterCharacter* Character = Cast<AMyBlasterCharacter>(GetOwningPlayerPawn()))
    {
        if (ABlasterPlayerState* PS = Cast<ABlasterPlayerState>(Character->GetPlayerState()))
        {
            // 슬롯 1 이미지 업데이트
            if (Slot1Image)
            {
                EWeaponType Slot1Weapon = PS->GetWeaponInSlot(0);
                if (Slot1Weapon != EWeaponType::EWT_MAX)
                {
                    // 무기 이미지 가져오기 (데이터 테이블 등에서)
                    if (UTexture2D* WeaponTexture = GetWeaponTexture(Slot1Weapon))
                    {
                        Slot1Image->SetBrushFromTexture(WeaponTexture);
                    }
                }
                else
                {
                    // 빈 슬롯 이미지
                    Slot1Image->SetBrushFromTexture(nullptr);
                }
            }

            // 슬롯 2 이미지 업데이트
            if (Slot2Image)
            {
                EWeaponType Slot2Weapon = PS->GetWeaponInSlot(1);
                if (Slot2Weapon != EWeaponType::EWT_MAX)
                {
                    if (UTexture2D* WeaponTexture = GetWeaponTexture(Slot2Weapon))
                    {
                        Slot2Image->SetBrushFromTexture(WeaponTexture);
                    }
                }
                else
                {
                    Slot2Image->SetBrushFromTexture(nullptr);
                }
            }
        }
    }
}

UTexture2D* UShop::GetWeaponTexture(EWeaponType WeaponType) const
{
    if (!ShopWeaponTable) return nullptr;

    // 데이터 테이블의 모든 행을 순회
    for (auto It : ShopWeaponTable->GetRowMap())
    {
        FWeaponData* WeaponData = (FWeaponData*)It.Value;
        if (WeaponData && WeaponData->WeaponType == WeaponType)
        {
            return WeaponData->WeaponImage.LoadSynchronous();
        }
    }

    return nullptr;
}

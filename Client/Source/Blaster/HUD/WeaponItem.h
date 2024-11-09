

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/ShopStructs.h"
#include "WeaponItem.generated.h"



class UImage;
class UTextBlock;
class UButton;
class UHorizontalBox;

/**
 * 
 */
UCLASS()
class BLASTER_API UWeaponItem : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
	void SetWeaponData(const FWeaponData& InWeaponData);
    void UpdatePurchaseState(bool bIsPurchased);

private:
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* WeaponNameText;

    UPROPERTY(meta = (BindWidget))
    UImage* WeaponImage;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PriceText;

    FWeaponData WeaponData;

    /**
    * 구매 관련
    */

    UPROPERTY(meta = (BindWidget))
    UButton* BuyButton;

    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* PriceInfo;

    UPROPERTY(meta = (BindWidget))
    UButton* Slot1Button;

    UPROPERTY(meta = (BindWidget))
    UButton* Slot2Button;

    UPROPERTY()
    class UShopComponent* ShopComponent;

    UFUNCTION()
    void OnBuyClicked();

    UFUNCTION()
    void OnSlot1Clicked();

    UFUNCTION()
    void OnSlot2Clicked();
	
    UFUNCTION()
    void OnWeaponPurchaseStateChanged(EWeaponType PurchasedWeaponType);
};

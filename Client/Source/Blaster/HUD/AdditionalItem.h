

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/ShopStructs.h"
#include "AdditionalItem.generated.h"

class UImage;
class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class BLASTER_API UAdditionalItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;
	void SetThrowData(const FThrowData& InThrowData);
	void SetBuffData(const FBuffData& InBuffData);

    void UpdatePurchaseState(bool bIsPurchased);

private:

    bool bIsThrowable; // 어떤 타입인지 구분지을려고 추가

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemNameText;

    UPROPERTY(meta = (BindWidget))
    UImage* ItemImage;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* OwnedText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PriceText;

    UPROPERTY(meta = (BindWidget))
    UButton* BuyButton;

    FThrowData ThrowData;
    FBuffData BuffData;

    UPROPERTY()
    class UShopComponent* ShopComponent;

    UFUNCTION()
    void OnBuyClicked();

    UFUNCTION()
    void OnBuffStateChanged(EBuffType BuffType, bool bActive);
    
};

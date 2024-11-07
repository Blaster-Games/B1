

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/ShopStructs.h"
#include "WeaponItem.generated.h"



class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class BLASTER_API UWeaponItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetWeaponData(const FWeaponData& InWeaponData);

private:
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* WeaponNameText;

    UPROPERTY(meta = (BindWidget))
    UImage* WeaponImage;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PriceText;

    FWeaponData WeaponData;

	
};

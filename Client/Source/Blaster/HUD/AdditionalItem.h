

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/ShopStructs.h"
#include "AdditionalItem.generated.h"

class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class BLASTER_API UAdditionalItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetThrowData(const FThrowData& InThrowData);
	void SetBuffData(const FBuffData& InBuffData);

private:

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemNameText;

    UPROPERTY(meta = (BindWidget))
    UImage* ItemImage;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PriceText;

    FThrowData ThrowData;
    FBuffData BuffData;

};

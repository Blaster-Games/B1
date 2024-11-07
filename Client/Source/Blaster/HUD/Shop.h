

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shop.generated.h"



class UUniformGridPanel;
class UDataTable;

/**
 * 
 */
UCLASS()
class BLASTER_API UShop : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
private:

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* WeaponGrid;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* ThrowGrid;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* BuffGrid;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	UDataTable* ShopWeaponTable;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	UDataTable* ShopThrowTable;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	UDataTable* ShopBuffTable;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	TSubclassOf<UUserWidget> WeaponItemWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	TSubclassOf<UUserWidget> AdditionalItemWidgetClass;

	void CreateWeaponItems();
	void CreateThrowItems();
	void CreateBuffItems();
};

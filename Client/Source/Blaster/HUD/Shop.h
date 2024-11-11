

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Shop.generated.h"


class UTextBlock;
class UUniformGridPanel;
class UDataTable;
class UImage;

/**
 * 
 */
UCLASS()
class BLASTER_API UShop : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WarmupTime;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoneyText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BlueTeamScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RedTeamScore;
	
	void ShowShop();
	void HideShop();  
	void InitializeShop();

private:

	UPROPERTY(meta = (BindWidget))
	UImage* Slot1Image;

	UPROPERTY(meta = (BindWidget))
	UImage* Slot2Image;

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

	UPROPERTY()
	class APlayerController* PlayerController;

	void CreateWeaponItems();
	void CreateThrowItems();
	void CreateBuffItems();

	UPROPERTY()
	class ABlasterPlayerState* PlayerState;

	UFUNCTION()
	void UpdateMoneyText(int32 NewMoney);

	UFUNCTION()
	void UpdateSlotImages();

	UTexture2D* GetWeaponTexture(EWeaponType WeaponType) const;

};

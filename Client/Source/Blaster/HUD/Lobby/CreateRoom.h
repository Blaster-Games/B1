#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ComboBoxString.h"
#include "CreateRoom.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomCreatedDelegate, const FRoomDetailInfo&, RoomInfo);
/**
 * 
 */
UCLASS()
class BLASTER_API UCreateRoom : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Room")
	FOnRoomCreatedDelegate OnRoomCreated;
	
protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* TitleInput;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* GameModeComboBox;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* MaxPlayersComboBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmCreateButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* RootOverlay;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackgroundButton;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* ModalContent;

	UFUNCTION()
	void OnBackgroundClicked();

	UFUNCTION()
	void OnConfirmCreateClicked();

	UFUNCTION()
	void OnCancelClicked();

	UFUNCTION()
	void HandleCreateRoomResponse(const bool Success, const FRoomDetailInfo& RoomInfo);

};

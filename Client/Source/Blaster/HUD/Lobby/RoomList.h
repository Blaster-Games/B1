#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "RoomItem.h"
#include "RoomList.generated.h"

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomSelectedDelegate, int32, RoomId);

class URoomItem;

UCLASS()
class BLASTER_API URoomList : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	// UI 컴포넌트들
	UPROPERTY(meta = (BindWidget))
	class UButton* CreateRoomButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* UpdateRoomItemButton;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* RoomListScrollBox;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<URoomItem> RoomItemClass;

	UPROPERTY()
	TArray<URoomItem*> RoomItems;

	// 이벤트 핸들러
	UFUNCTION()
	void OnCreateRoomClicked();

	UFUNCTION()
	void OnUpdateRoomItemClicked();

	UFUNCTION()
	void HandleRoomListResponse(const TArray<FRoomListItemInfo>& Rooms);

	UFUNCTION()
	void HandleRoomItemDoubleClicked(int32 RoomId);

public:
	// 방 목록 관리 함수들
	void UpdateRoomList(const TArray<FRoomListItemInfo>& Rooms);
	URoomItem* AddRoom(const FRoomListItemInfo& RoomInfo);
	void ClearRoomList();

	// Room 선택 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Room Events")
	FOnRoomSelectedDelegate OnRoomSelected;
};
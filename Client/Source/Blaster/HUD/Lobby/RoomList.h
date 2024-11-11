#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "RoomItem.h"
#include "RoomList.generated.h"

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

	// RoomItem 배열
	UPROPERTY()
	TArray<URoomItem*> RoomItems;

	// 방 생성 버튼 클릭 이벤트
	UFUNCTION()
	void OnCreateRoomClicked();

	UFUNCTION()
	void OnUpdateRoomItemClicked();

	UFUNCTION()
	void HandleRoomListResponse(const TArray<FRoomListItemInfo>& Rooms);

public:
	// 방 목록 관리 함수들
	void UpdateRoomList(const TArray<FRoomListItemInfo>& Rooms);
	URoomItem* AddRoom(const FRoomListItemInfo& RoomInfo);
	void ClearRoomList();
};
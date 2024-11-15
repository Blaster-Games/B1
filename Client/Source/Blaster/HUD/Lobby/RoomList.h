#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Room/RoomDetail.h"
#include "RoomItem.h"
#include "CreateRoom.h"
#include "Protocol.pb.h"
#include "RoomList.generated.h"

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomSelectedDelegate, int32, RoomId);

class URoomItem;
class UCreateRoom;
class URoomDetail;

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

	// 위젯 클래스 레퍼런스들
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<URoomItem> RoomItemClass;

	// 방 아이템 배열
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<URoomDetail> RoomDetailWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCreateRoom> CreateRoomWidgetClass;

	// 방 목록 갱신 요청
	UFUNCTION()
	void RequestUpdateRoomList();

	// 네트워크 핸들러 설정
	void SetNetworkHandler(class INetworkHandler* InHandler) { NetworkHandler = InHandler; }

	UPROPERTY(BlueprintAssignable, Category = "Room Events")
	FOnRoomSelectedDelegate OnRoomSelected;

	UFUNCTION()
	void HandleCreateRoomResponse(const FRoomDetailInfo& RoomInfo);

private:
	// 네트워크 핸들러
	class INetworkHandler* NetworkHandler;
};
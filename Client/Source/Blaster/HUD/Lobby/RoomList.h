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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomSelectedDelegate, int32, RoomId);

class UBlasterNetworkSubsystem;

UCLASS()
class BLASTER_API URoomList : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    // UI 컴포넌트
    UPROPERTY(meta = (BindWidget))
    class UButton* CreateRoomButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* UpdateRoomItemButton;

    UPROPERTY(meta = (BindWidget))
    class UScrollBox* RoomListScrollBox;

    // 위젯 클래스들
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<URoomItem> RoomItemClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<URoomDetail> RoomDetailWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UCreateRoom> CreateRoomWidgetClass;

    // 캐시된 방 아이템들
    UPROPERTY()
    TArray<URoomItem*> RoomItems;

    // 서브시스템 참조
    UPROPERTY()
    mutable UBlasterNetworkSubsystem* NetworkSubsystem;
    UBlasterNetworkSubsystem* GetNetworkSubsystem() const;

    // 이벤트 핸들러들
    UFUNCTION()
    void OnCreateRoomClicked();

    UFUNCTION()
    void OnUpdateRoomItemClicked();

    UFUNCTION()
    void HandleRoomListResponse(const TArray<FRoomListItemInfo>& Rooms);

    UFUNCTION()
    void HandleRoomItemDoubleClicked(int32 RoomId);

    UFUNCTION()
    void HandleCreateRoomResponse(const FRoomDetailInfo& RoomInfo);

public:
    // Public API
    void UpdateRoomList(const TArray<FRoomListItemInfo>& Rooms);
    URoomItem* AddRoom(const FRoomListItemInfo& RoomInfo);
    void ClearRoomList();

    UFUNCTION()
    void RequestUpdateRoomList();

    // 이벤트
    UPROPERTY(BlueprintAssignable, Category = "Room Events")
    FOnRoomSelectedDelegate OnRoomSelected;
};
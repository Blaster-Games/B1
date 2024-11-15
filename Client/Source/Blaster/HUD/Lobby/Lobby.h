#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Room/RoomDetail.h"
#include "Lobby.generated.h"

UCLASS()
class BLASTER_API ULobby : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    class URoomList* RoomListWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<URoomDetail> RoomWidgetClass;

    UFUNCTION()
    void HandleRoomItemDoubleClicked(int32 RoomId);

    UFUNCTION()
    void HandleJoinRoomResponse(bool Success, const FRoomDetailInfo& RoomInfo);

    void ShowRoomWidget(const FRoomDetailInfo& RoomInfo);

public:

    URoomList* GetRoomList() const { return RoomListWidget; }
};
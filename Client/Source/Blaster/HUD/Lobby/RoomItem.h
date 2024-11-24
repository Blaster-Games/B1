#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "RoomTypes.h"
#include "RoomItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomItemClicked, int32, RoomId);

UCLASS()
class BLASTER_API URoomItem : public UUserWidget
{
    GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RoomIdText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RoomNameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RoomTypeText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RoomPlayerCountText;

    UPROPERTY(meta = (BindWidget))
    class UButton* RoomItemButton;

    UFUNCTION()
    void OnRoomItemButtonClicked();

    UFUNCTION()
	void OnRoomClicked();

    // 룸 ID 저장
    int32 RoomId;

    float LastClickTime = 0.0f;
    const float DoubleClickTime = 0.2f;

public:
    void SetRoomInfo(
        int32 InRoomId,
        const FString& InRoomName,
        EGameMode InRoomType,
        int32 InCurrentPlayers,
        int32 InMaxPlayers,
        ERoomState InState,
        const FString& InMapName
    );

    // 클릭 이벤트 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Room Events")
    FOnRoomItemClicked OnRoomItemClicked;
};
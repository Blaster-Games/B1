
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomPlayers.h"
#include "../RoomTypes.h"
#include "Components/TextBlock.h"
#include "RoomDetail.generated.h"

UCLASS()
class BLASTER_API URoomDetail : public UUserWidget
{
    GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;

    // UI 컴포넌트들
    UPROPERTY(meta = (BindWidget))
    URoomPlayers* RoomPlayers;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RoomNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RoomTypeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PlayerCountText;

private:
    FRoomDetailInfo CurrentRoomInfo;

public:
    // 방 정보 업데이트
    void UpdateRoomInfo(const FRoomDetailInfo& RoomInfo);

    // UI 업데이트 헬퍼 함수들
private:
    void UpdateUI();
    FString GetRoomTypeString(EGameMode RoomType) const;
};
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "RoomTypes.h"
#include "RoomItem.generated.h"

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
};
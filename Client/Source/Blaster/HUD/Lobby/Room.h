#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomTypes.h"
#include "RoomPlayers.h"
#include "Components/TextBlock.h"
#include "Room.generated.h"

UCLASS()
class BLASTER_API URoom : public UUserWidget
{
    GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;
public:
    void UpdateRoom(const FRoomDetailInfo& RoomInfo);
};
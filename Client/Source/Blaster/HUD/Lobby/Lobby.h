#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomList.h"
#include "Lobby.generated.h"

UCLASS()
class BLASTER_API ULobby : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    class URoomList* RoomList;

public:

    URoomList* GetRoomList() const { return RoomList; }
};
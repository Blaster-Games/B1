#include "Room.h"
#include "Components/PanelWidget.h"

void URoom::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Log, TEXT("Room: Basic NativeConstruct called"));
    SetVisibility(ESlateVisibility::Visible);
}

void URoom::UpdateRoom(const FRoomDetailInfo& RoomInfo)
{
    UE_LOG(LogTemp, Log, TEXT("Room: Basic UpdateRoom called"));
}
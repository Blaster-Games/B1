
#include "Room.h"

void URoom::NativeConstruct()
{
	Super::NativeConstruct();
}

void URoom::UpdateRoom(const FRoomDetailInfo& RoomInfo)
{
	CurrentRoomInfo = RoomInfo;

	if (RoomPlayers)
	{
		RoomPlayers->UpdatePlayers(RoomInfo.Players);
	}
}

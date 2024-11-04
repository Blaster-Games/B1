#include "ClientPacketHandler.h"
#include "BufferReader.h"


PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_Invalid(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_S_Connected(PacketSessionRef& session, Protocol::S_Connected& pkt)
{
	return true;
}

#include "BlasterGameInstance.h"
#include "GameInstance/BlasterNetworkSubsystem.h"

void UBlasterGameInstance::Init()
{
    Super::Init();

    NetworkSystem = GetSubsystem<UBlasterNetworkSubsystem>();
}

void UBlasterGameInstance::ConnectToServer()
{
    if (NetworkSystem)
    {
        NetworkSystem->ConnectToGameServer();
    }
}

void UBlasterGameInstance::DisconnectFromServer()
{
    if (NetworkSystem)
    {
        NetworkSystem->DisconnectFromGameServer();
    }
}

void UBlasterGameInstance::HandleNetworkPackets()
{
    if (NetworkSystem)
    {
        NetworkSystem->HandleRecvPackets();
    }
}

void UBlasterGameInstance::SendNetworkPacket(SendBufferRef SendBuffer)
{
    if (NetworkSystem)
    {
        NetworkSystem->SendPacket(SendBuffer);
    }
}
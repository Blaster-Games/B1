
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blaster.h"
#include "BlasterGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();
	
	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(SendBufferRef SendBuffer);

public:
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1"); // TEMP
	int16 Port = 7777;
	TSharedPtr<class PacketSession> GameServerSession;
};

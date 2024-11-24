#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OutGameMode.generated.h"

UCLASS()
class BLASTER_API AOutGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOutGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void BeginPlay() override;

private:
	// CurrentRoomInfo를 GameInstance에서 가져와서 체크하는 함수
	void CheckAndStartGame();
};
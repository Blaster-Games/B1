#include "OutGameMode.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameState.h"
#include "GameInstance/BlasterGameInstance.h"
#include "../HUD/Lobby/RoomTypes.h"
#include "Kismet/GameplayStatics.h"

AOutGameMode::AOutGameMode()
{
    // BP_OutGamePlayerController는 이미 블루프린트에서 설정되어 있음
}

void AOutGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (GetNetMode() == NM_ListenServer)
    {
        UE_LOG(LogTemp, Log, TEXT("Player logged in to listen server"));
        CheckAndStartGame();
    }
}

void AOutGameMode::CheckAndStartGame()
{
    // 현재 접속한 플레이어 수 확인
    int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

    // GameInstance에서 현재 방 정보 가져오기
    if (UBlasterGameInstance* GameInstance = Cast<UBlasterGameInstance>(GetGameInstance()))
    {
        const FRoomDetailInfo& RoomInfo = GameInstance->GetCurrentRoomInfo();

        UE_LOG(LogTemp, Log, TEXT("Checking players: %d/%d"), NumberOfPlayers, RoomInfo.MaxPlayers);

        // 필요한 플레이어 수가 모였는지 확인
        if (NumberOfPlayers >= RoomInfo.MaxPlayers)
        {
            UE_LOG(LogTemp, Log, TEXT("All players connected. Starting game..."));

            // 게임 맵으로 이동
            FString GameMapPath;
            if (RoomInfo.MapName.StartsWith(TEXT("/Game/")))
            {
                GameMapPath = FString::Printf(TEXT("%s?listen"), *RoomInfo.MapName);
            }
            else
            {
                GameMapPath = FString::Printf(TEXT("/Game/Maps/%s?listen"), *RoomInfo.MapName);
            }

            // 심리스 트래블 사용
            bUseSeamlessTravel = true;

            // 서버 트래블 실행
            GetWorld()->ServerTravel(GameMapPath);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get BlasterGameInstance"));
    }
}
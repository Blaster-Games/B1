#include "OutGameMode.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameState.h"
#include "GameInstance/BlasterGameInstance.h"
#include "../HUD/Lobby/RoomTypes.h"
#include "PlayerController/OutGamePlayerController.h"
#include "Kismet/GameplayStatics.h"

AOutGameMode::AOutGameMode()
{
    PlayerControllerClass = AOutGamePlayerController::StaticClass();
}

void AOutGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    UE_LOG(LogTemp, Log, TEXT("PostLogin called - Player: %s, NetMode: %d"),
        *GetNameSafe(NewPlayer), static_cast<int32>(GetNetMode()));

    if (GetNetMode() == NM_ListenServer)
    {
        if (GameState.Get()->PlayerArray.Num() == 1)
        {
            UE_LOG(LogTemp, Log, TEXT("Host logged in to listen server"));
            // 호스트 관련 초기화 코드
        }

        // 현재 접속한 플레이어가 누구인지 확인
        if (AOutGamePlayerController* PC = Cast<AOutGamePlayerController>(NewPlayer))
        {
            UE_LOG(LogTemp, Log, TEXT("Player logged in to listen server. PlayerNum: %d"),
                GameState.Get()->PlayerArray.Num());
        }

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

        UE_LOG(LogTemp, Log, TEXT("Current Map Name from RoomInfo: '%s'"), *RoomInfo.MapName);

        // 필요한 플레이어 수가 모였는지 확인
        if (NumberOfPlayers >= RoomInfo.MaxPlayers)
        {
            UE_LOG(LogTemp, Log, TEXT("All players connected. Starting game..."));

            // URL에 게임모드 파라미터 추가
            FString GameMapPath;
            if (RoomInfo.MapName.StartsWith(TEXT("/Game/")))
            {
                GameMapPath = FString::Printf(TEXT("%s?listen?game=/Game/Blueprints/GameModes/BP_BlasterGameMode"), *RoomInfo.MapName);
            }
            else
            {
                GameMapPath = FString::Printf(TEXT("/Game/Maps/%s?listen?game=/Game/Blueprints/GameModes/BP_BlasterGameMode"), *RoomInfo.MapName);
            }

            bUseSeamlessTravel = false;
            GetWorld()->ServerTravel(GameMapPath);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get BlasterGameInstance"));
    }
}
#include "OutGameMode.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameState.h"
#include "GameInstance/BlasterGameInstance.h"
#include "../HUD/Lobby/RoomTypes.h"
#include "PlayerController/OutGamePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/BlasterPlayerState.h"

AOutGameMode::AOutGameMode()
{
    PlayerControllerClass = AOutGamePlayerController::StaticClass();

	PlayerStateClass = ABlasterPlayerState::StaticClass();

    UE_LOG(LogTemp, Log, TEXT("OutGameMode Constructor - PlayerStateClass: %s"),
        PlayerStateClass ? *PlayerStateClass->GetName() : TEXT("Not Set"));
}

void AOutGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // 현재 네트워크 모드 확인
    ENetMode NetMode = GetNetMode();
    UE_LOG(LogTemp, Log, TEXT("PostLogin called. NetMode: %d, Player: %s"),
        (int32)NetMode, *NewPlayer->GetName());

    if (NetMode == NM_Standalone)
    {
        UE_LOG(LogTemp, Log, TEXT("This is standalone mode (PIE or packaged)"));
        // 독립 실행/PIE 시작시
        return;  // 여기서 리턴하면 닉네임 설정 등의 멀티플레이어 로직은 실행 안 됨
    }

    // 리슨서버에서의 로직
    if (NetMode == NM_ListenServer)
    {
        UE_LOG(LogTemp, Log, TEXT("This is listen server. PlayerCount: %d"),
            GameState.Get()->PlayerArray.Num());

        if (NewPlayer)
        {
            UE_LOG(LogTemp, Log, TEXT("NewPlayer is valid: %s"), *NewPlayer->GetName());

            // PlayerState 체크
            APlayerState* BasePS = NewPlayer->GetPlayerState<APlayerState>();
            UE_LOG(LogTemp, Log, TEXT("Base PlayerState: %s"),
                BasePS ? *BasePS->GetName() : TEXT("nullptr"));

            ABlasterPlayerState* PS = NewPlayer->GetPlayerState<ABlasterPlayerState>();
            UE_LOG(LogTemp, Log, TEXT("BlasterPlayerState cast result: %s"),
                PS ? *PS->GetName() : TEXT("nullptr"));

            if (PS)
            {
                UGameInstance* BaseGI = GetGameInstance();
                UE_LOG(LogTemp, Log, TEXT("Base GameInstance: %s"),
                    BaseGI ? *BaseGI->GetName() : TEXT("nullptr"));

                UBlasterGameInstance* GI = Cast<UBlasterGameInstance>(GetGameInstance());
                UE_LOG(LogTemp, Log, TEXT("BlasterGameInstance cast result: %s"),
                    GI ? *GI->GetName() : TEXT("nullptr"));

                if (GI)
                {
                    FString CurrentNickname = GI->GetNickname();
                    UE_LOG(LogTemp, Log, TEXT("Retrieved Nickname from GI: %s"), *CurrentNickname);

                    PS->SetNickname(CurrentNickname);
                    UE_LOG(LogTemp, Log, TEXT("Nickname set on PS. PlayerState: %s, Nickname: %s"),
                        *PS->GetName(), *PS->GetNickname());  // PS에서 닉네임도 확인
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Failed to cast to BlasterGameInstance"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to cast to BlasterPlayerState"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("NewPlayer is nullptr"));
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

            bUseSeamlessTravel = true;
            GetWorld()->ServerTravel(GameMapPath);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get BlasterGameInstance"));
    }
}
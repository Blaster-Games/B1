#include "RoomDetail.h"
#include "GameInstance/BlasterNetworkSubsystem.h"
#include "GameInstance/BlasterGameInstance.h"


void URoomDetail::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Log, TEXT("RoomDetail: NativeConstruct called"));

    // GameInstance에서 현재 방 정보 가져오기
    if (UBlasterGameInstance* GameInstance = Cast<UBlasterGameInstance>(GetGameInstance()))
    {
        // 초기 UI 업데이트
        UpdateUI();

        // 이미 설정된 플레이어 정보가 있다면 업데이트
        if (RoomPlayers)
        {
            const FRoomDetailInfo& RoomInfo = GameInstance->GetCurrentRoomInfo();
            if (RoomInfo.Players.Num() > 0)
            {
                RoomPlayers->UpdatePlayers(RoomInfo.Players, RoomInfo.MaxPlayers);
                UE_LOG(LogTemp, Log, TEXT("RoomDetail: Initialize players list with %d players"), RoomInfo.Players.Num());
            }
        }
    }

    // 버튼 이벤트 바인딩
    if (RedTeamButton)
    {
        RedTeamButton->OnClicked.AddDynamic(this, &URoomDetail::OnRedTeamButtonClicked);
    }
    if (BlueTeamButton)
    {
        BlueTeamButton->OnClicked.AddDynamic(this, &URoomDetail::OnBlueTeamButtonClicked);
    }
    if (StartGameButton)
    {
        StartGameButton->OnClicked.AddDynamic(this, &URoomDetail::OnStartGameButtonClicked);
    }
    if (LeaveGameButton)
    {
        // TODO: LeaveGameButton 클릭 이벤트 바인딩
    }

    // 네트워크 이벤트 바인딩
    if (UBlasterNetworkSubsystem* NS = GetNetworkSubsystem())
    {
        NS->OnBroadcastJoinRoom.AddDynamic(this, &URoomDetail::UpdateRoomInfo);
		NS->OnBroadcastStartGame.AddDynamic(this, &URoomDetail::TravelToHostServer);
    }
}
void URoomDetail::NativeDestruct()
{
    NetworkSubsystem = nullptr;
    Super::NativeDestruct();
}

UBlasterNetworkSubsystem* URoomDetail::GetNetworkSubsystem() const
{
    if (NetworkSubsystem == nullptr)
    {
        if (UGameInstance* GameInstance = GetGameInstance())
        {
            NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>();
        }
    }
    return NetworkSubsystem;
}

void URoomDetail::UpdateRoomInfo(const FRoomDetailInfo& RoomInfo)
{
    // GameInstance에 방 정보 업데이트
    if (UBlasterGameInstance* GameInstance = Cast<UBlasterGameInstance>(GetGameInstance()))
    {
        GameInstance->UpdateCurrentRoomInfo(RoomInfo);

        UE_LOG(LogTemp, Log, TEXT("RoomDetail: Updating room info for Room ID: %d"), RoomInfo.RoomId);

        // UI 업데이트는 다음 틱에서 실행
        GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
            {
                UpdateUI();

                // 플레이어 리스트 업데이트
                if (RoomPlayers)
                {
                    // GameInstance에서 최신 정보 가져오기
                    const FRoomDetailInfo& CurrentInfo = Cast<UBlasterGameInstance>(GetGameInstance())->GetCurrentRoomInfo();
                    RoomPlayers->UpdatePlayers(CurrentInfo.Players, CurrentInfo.MaxPlayers);
                    UE_LOG(LogTemp, Log, TEXT("RoomDetail: Updated players list with %d players"), CurrentInfo.Players.Num());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("RoomDetail: RoomPlayers widget is null"));
                }
            });
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("RoomDetail: Failed to get BlasterGameInstance"));
    }
}

void URoomDetail::TravelToHostServer(const FString& HostAddress, int32 HostPort)
{
    UE_LOG(LogTemp, Log, TEXT("Attempting to join host server at %s:%d"), *HostAddress, HostPort);

    if (UBlasterGameInstance* GameInstance = Cast<UBlasterGameInstance>(GetGameInstance()))
    {
        GameInstance->JoinGame(HostAddress, HostPort);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get GameInstance while trying to join host server"));
    }
}

void URoomDetail::UpdateUI()
{
    if (UBlasterGameInstance* GameInstance = Cast<UBlasterGameInstance>(GetGameInstance()))
    {
        const FRoomDetailInfo& CurrentInfo = GameInstance->GetCurrentRoomInfo();

        if (RoomNameText)
        {
            RoomNameText->SetText(FText::FromString(CurrentInfo.RoomName));
        }

        if (RoomTypeText)
        {
            RoomTypeText->SetText(FText::FromString(GetRoomTypeString(CurrentInfo.RoomType)));
        }

        if (PlayerCountText)
        {
            FString PlayerCountString = FString::Printf(TEXT("%d/%d"), CurrentInfo.Players.Num(), CurrentInfo.MaxPlayers);
            PlayerCountText->SetText(FText::FromString(PlayerCountString));
        }

        // 시작 버튼 가시성 설정
        if (StartGameButton)
        {
            // 현재 플레이어가 호스트인지 확인
            bool bIsHost = false;
            int32 MyPlayerId = GameInstance->GetPlayerId(); // GetPlayerId() 함수가 필요합니다

            // Players 배열에서 현재 플레이어를 찾아 호스트 여부 확인
            for (const FPlayerInfo& Player : CurrentInfo.Players)
            {
                if (Player.PlayerId == MyPlayerId)
                {
                    bIsHost = Player.IsHost;
                    break;
                }
            }

            // 호스트일 경우만 버튼 표시
            StartGameButton->SetVisibility(bIsHost ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
            UE_LOG(LogTemp, Log, TEXT("Start button visibility set to: %s"), bIsHost ? TEXT("Visible") : TEXT("Collapsed"));
        }
    }
}

FString URoomDetail::GetRoomTypeString(EGameMode RoomType) const
{
    switch (RoomType)
    {
    case EGameMode::MODE_DEATHMATCH:
        return TEXT("데스매치");
    case EGameMode::MODE_TEAM_DEATHMATCH:
        return TEXT("팀 데스매치");
    default:
        return TEXT("알 수 없음");
    }
}

void URoomDetail::OnRedTeamButtonClicked()
{
    RequestTeamChange();
}

void URoomDetail::OnBlueTeamButtonClicked()
{
    RequestTeamChange();
}

void URoomDetail::RequestTeamChange()
{
	// TODO : 팀 변경 요청 코드 구현
}

void URoomDetail::OnStartGameButtonClicked()
{
    if (UBlasterGameInstance* GameInstance = Cast<UBlasterGameInstance>(GetGameInstance()))
    {
        // 현재 맵에서 리슨서버 시작
        GameInstance->HostGame();
        UE_LOG(LogTemp, Log, TEXT("Starting listen server in current map"));
    }
}

void URoomDetail::OnLeaveGameButtonClicked()
{
    // 게임 나가기 버튼 클릭 시 실행할 코드 구현
}
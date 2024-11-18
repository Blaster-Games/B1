#include "RoomDetail.h"
#include "GameInstance/BlasterNetworkSubsystem.h"


void URoomDetail::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Log, TEXT("RoomDetail: NativeConstruct called"));

    // 초기 UI 업데이트
    UpdateUI();

    // 이미 설정된 플레이어 정보가 있다면 업데이트
    if (RoomPlayers && CurrentRoomInfo.Players.Num() > 0)
    {
        RoomPlayers->UpdatePlayers(CurrentRoomInfo.Players);
        UE_LOG(LogTemp, Log, TEXT("RoomDetail: Initialize players list with %d players"), CurrentRoomInfo.Players.Num());
    }

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
		// TODO: StartGameButton 클릭 이벤트 바인딩
    }

    if (LeaveGameButton)
    {
		// TODO: LeaveGameButton 클릭 이벤트 바인딩
    }

    if (UBlasterNetworkSubsystem* NS = GetNetworkSubsystem())
    {
        NS->OnBroadcastJoinRoom.AddDynamic(this, &URoomDetail::UpdateRoomInfo);
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
    CurrentRoomInfo = RoomInfo;
    UE_LOG(LogTemp, Log, TEXT("RoomDetail: Updating room info for Room ID: %d"), RoomInfo.RoomId);

    // NativeConstruct 이후에 호출되도록 Tick에서 한번만 업데이트하도록 수정
    GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
        {
            UpdateUI();

            // 플레이어 리스트 업데이트
            if (RoomPlayers)
            {
                RoomPlayers->UpdatePlayers(CurrentRoomInfo.Players);
                UE_LOG(LogTemp, Log, TEXT("RoomDetail: Updated players list with %d players"), CurrentRoomInfo.Players.Num());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("RoomDetail: RoomPlayers widget is null"));
            }
        });
}

void URoomDetail::UpdateUI()
{
    // 방 이름 업데이트
    if (RoomNameText)
    {
        RoomNameText->SetText(FText::FromString(CurrentRoomInfo.RoomName));
        UE_LOG(LogTemp, Log, TEXT("RoomDetail: Updated room name to: %s"), *CurrentRoomInfo.RoomName);
    }

    // 방 타입 업데이트
    if (RoomTypeText)
    {
        FString RoomTypeStr = GetRoomTypeString(CurrentRoomInfo.RoomType);
        RoomTypeText->SetText(FText::FromString(RoomTypeStr));
    }

    // 플레이어 수 업데이트
    if (PlayerCountText)
    {
        FString PlayerCountStr = FString::Printf(TEXT("%d/%d"),
            CurrentRoomInfo.Players.Num(),
            CurrentRoomInfo.MaxPlayers);
        PlayerCountText->SetText(FText::FromString(PlayerCountStr));
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

}

void URoomDetail::OnStartGameButtonClicked()
{
    // 게임 시작 버튼 클릭 시 실행할 코드 구현
}

void URoomDetail::OnLeaveGameButtonClicked()
{
    // 게임 나가기 버튼 클릭 시 실행할 코드 구현
}
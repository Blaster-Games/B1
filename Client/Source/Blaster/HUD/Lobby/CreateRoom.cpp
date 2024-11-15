#include "CreateRoom.h"
#include "Components/EditableTextBox.h"
#include "GameInstance/BlasterNetworkSubsystem.h"
#include "Components/Button.h"

void UCreateRoom::NativeConstruct()
{
    Super::NativeConstruct();

    if (GameModeComboBox)
    {
        GameModeComboBox->AddOption(TEXT("None"));           // index 0 = MODE_NONE
        GameModeComboBox->AddOption(TEXT("데스매치"));       // index 1 = MODE_DEATHMATCH
        GameModeComboBox->AddOption(TEXT("팀 데스매치"));    // index 2 = MODE_TEAMDEATHMATCH
        GameModeComboBox->SetSelectedIndex(2);  // 기본값을 팀 데스매치로 설정
    }

    if (MaxPlayersComboBox)
    {
        MaxPlayersComboBox->AddOption(TEXT("4"));
        MaxPlayersComboBox->AddOption(TEXT("6"));
        MaxPlayersComboBox->AddOption(TEXT("8"));
        MaxPlayersComboBox->SetSelectedIndex(1);  // 기본값 6명
    }

    // 버튼 이벤트 바인딩
    if (ConfirmCreateButton)
    {
        ConfirmCreateButton->OnClicked.AddDynamic(this, &UCreateRoom::OnConfirmCreateClicked);
    }

    if (CancelButton)
    {
        CancelButton->OnClicked.AddDynamic(this, &UCreateRoom::OnCancelClicked);
    }

    if (BackgroundButton)
    {
        BackgroundButton->OnClicked.AddDynamic(this, &UCreateRoom::OnBackgroundClicked);
    }

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            NetworkSubsystem->OnConfirmCreateRoomResponse.AddDynamic(this, &UCreateRoom::HandleCreateRoomResponse);
        }
    }
}

FReply UCreateRoom::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::Escape)
    {
        RemoveFromParent();
        return FReply::Handled();
    }
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

UFUNCTION()
void UCreateRoom::OnConfirmCreateClicked()
{
    if (!TitleInput || TitleInput->GetText().IsEmpty()) return;

    FString Title = TitleInput->GetText().ToString();
    EGameMode GameMode = static_cast<EGameMode>(GameModeComboBox->GetSelectedIndex());
    int32 MaxPlayers = FCString::Atoi(*MaxPlayersComboBox->GetSelectedOption());

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            NetworkSubsystem->SendCreateRoomReq(Title, GameMode, MaxPlayers);
        }
    }
}

void UCreateRoom::HandleCreateRoomResponse(const bool Success, const FRoomDetailInfo& RoomInfo)
{
    if (Success)
    {
        // 방 생성 성공
        OnRoomCreated.Broadcast(RoomInfo);
        RemoveFromParent();
    }
    else
    {
        // 실패 처리
        // 예: 에러 메시지 표시
    }
}

void UCreateRoom::OnBackgroundClicked()
{
    RemoveFromParent();
}

UFUNCTION()
void UCreateRoom::OnCancelClicked()
{
    RemoveFromParent();
}
#include "RoomPlayers.h"

void URoomPlayers::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Log, TEXT("RoomPlayers: NativeConstruct called"));
    InitializePlayerTextBlocks();
}

void URoomPlayers::InitializePlayerTextBlocks()
{
    UE_LOG(LogTemp, Log, TEXT("RoomPlayers: Initializing player text blocks"));

    // TextBlock 포인터들을 배열에 추가
    PlayerNameTexts.Add(Player1NameText);
    PlayerNameTexts.Add(Player2NameText);
    PlayerNameTexts.Add(Player3NameText);
    PlayerNameTexts.Add(Player4NameText);
    PlayerNameTexts.Add(Player5NameText);
    PlayerNameTexts.Add(Player6NameText);
    PlayerNameTexts.Add(Player7NameText);
    PlayerNameTexts.Add(Player8NameText);

    // TextBlock 바인딩 확인
    int32 ValidTextBlocks = 0;
    for (int32 i = 0; i < PlayerNameTexts.Num(); ++i)
    {
        if (PlayerNameTexts[i])
        {
            ValidTextBlocks++;
            UE_LOG(LogTemp, Log, TEXT("RoomPlayers: Player%d text block is bound"), i + 1);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("RoomPlayers: Player%d text block is null"), i + 1);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("RoomPlayers: Found %d valid text blocks out of %d"), ValidTextBlocks, PlayerNameTexts.Num());
}

void URoomPlayers::UpdatePlayers(const TArray<FPlayerInfo>& Players, int32 MaxPlayers)
{
    if (PlayerNameTexts.Num() == 0)
    {
        InitializePlayerTextBlocks();
    }

    UE_LOG(LogTemp, Log, TEXT("RoomPlayers: Updating players list with %d players (Max: %d)"), Players.Num(), MaxPlayers);

    // 모든 슬롯 초기화
    ClearAllSlots();

    // 활성화할 슬롯 수 계산
    int32 SlotsPerTeam = FMath::CeilToInt(MaxPlayers / 2.0f);

    // 각 슬롯의 활성화/비활성화 상태 설정
    for (int32 i = 0; i < PlayerNameTexts.Num(); ++i)
    {
        if (PlayerNameTexts[i])
        {
            bool bIsRedTeam = IsRedTeamSlot(i);
            int32 TeamSlotIndex = GetTeamSlotIndex(i);

            // 해당 슬롯이 현재 MaxPlayers 설정에서 사용되어야 하는지 확인
            bool bShouldBeActive = TeamSlotIndex < SlotsPerTeam;

            if (bShouldBeActive)
            {
                PlayerNameTexts[i]->SetVisibility(ESlateVisibility::Visible);
                // 빈 슬롯 텍스트 표시
                FString SlotText = FString::Printf(TEXT("%s 팀 슬롯 %d - 비어있음"),
                    bIsRedTeam ? TEXT("Red") : TEXT("Blue"),
                    TeamSlotIndex + 1);
                PlayerNameTexts[i]->SetText(FText::FromString(SlotText));
            }
            else
            {
                PlayerNameTexts[i]->SetVisibility(ESlateVisibility::Collapsed);
            }
        }
    }

    // 플레이어 정보 설정
    for (const FPlayerInfo& Player : Players)
    {
        // 플레이어의 팀에 따라 적절한 슬롯 찾기
        int32 SlotIndex;
        if (Player.Team == ETeamType::TEAM_RED)
        {
            SlotIndex = Player.PlayerId % 4;  // Red팀 슬롯 (0-3)
        }
        else
        {
            SlotIndex = (Player.PlayerId % 4) + 4;  // Blue팀 슬롯 (4-7)
        }

        if (SlotIndex < PlayerNameTexts.Num() && PlayerNameTexts[SlotIndex])
        {
            FString PlayerText = FString::Printf(TEXT("%s 팀 슬롯 %d - %s%s"),
                IsRedTeamSlot(SlotIndex) ? TEXT("Red") : TEXT("Blue"),
                GetTeamSlotIndex(SlotIndex) + 1,
                *Player.PlayerName,
                Player.IsHost ? TEXT(" (방장)") : TEXT(""));

            PlayerNameTexts[SlotIndex]->SetText(FText::FromString(PlayerText));
        }
    }
}

void URoomPlayers::ClearAllSlots()
{
    for (auto* TextBlock : PlayerNameTexts)
    {
        if (TextBlock)
        {
            TextBlock->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}
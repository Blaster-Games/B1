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
    PlayerNameTexts.Add(Player0NameText);
    PlayerNameTexts.Add(Player1NameText);
    PlayerNameTexts.Add(Player2NameText);
    PlayerNameTexts.Add(Player3NameText);
    PlayerNameTexts.Add(Player4NameText);
    PlayerNameTexts.Add(Player5NameText);
    PlayerNameTexts.Add(Player6NameText);
    PlayerNameTexts.Add(Player7NameText);

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

    // 들어오는 플레이어 정보 로깅
    UE_LOG(LogTemp, Log, TEXT("======= 플레이어 정보 시작 ======="));
    for (const FPlayerInfo& Player : Players)
    {
        UE_LOG(LogTemp, Log, TEXT("받은 플레이어 정보: Name=%s, SlotNumber=%d, Team=%d, IsHost=%s"),
            *Player.PlayerName,
            Player.SlotNumber,
            static_cast<int32>(Player.Team),
            Player.IsHost ? TEXT("true") : TEXT("false"));
    }
    UE_LOG(LogTemp, Log, TEXT("======= 플레이어 정보 끝 ======="));

    UE_LOG(LogTemp, Log, TEXT("RoomPlayers: Updating players list with %d players (Max: %d)"), Players.Num(), MaxPlayers);

    ClearAllSlots();

    int32 SlotsPerTeam = FMath::CeilToInt(MaxPlayers / 2.0f);
    UE_LOG(LogTemp, Log, TEXT("SlotsPerTeam: %d"), SlotsPerTeam);

    // 각 슬롯의 활성화/비활성화 상태 설정
    for (int32 i = 0; i < PlayerNameTexts.Num(); ++i)
    {
        if (PlayerNameTexts[i])
        {
            bool bIsRedTeam = IsRedTeamSlot(i);
            int32 TeamSlotIndex = GetTeamSlotIndex(i);
            bool bShouldBeActive = TeamSlotIndex < SlotsPerTeam;

            UE_LOG(LogTemp, Log, TEXT("슬롯 %d 초기화: IsRedTeam=%s, TeamSlotIndex=%d, ShouldBeActive=%s"),
                i,
                bIsRedTeam ? TEXT("true") : TEXT("false"),
                TeamSlotIndex,
                bShouldBeActive ? TEXT("true") : TEXT("false"));

            if (bShouldBeActive)
            {
                PlayerNameTexts[i]->SetVisibility(ESlateVisibility::Visible);
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

    // 플레이어 정보 설정 시 상세 로깅
    for (const FPlayerInfo& Player : Players)
    {
        int32 SlotIndex = Player.SlotNumber;
        UE_LOG(LogTemp, Log, TEXT("플레이어 설정 시도: Name=%s, SlotNumber=%d"),
            *Player.PlayerName, SlotIndex);

        if (SlotIndex < PlayerNameTexts.Num() && PlayerNameTexts[SlotIndex])
        {
            FString PlayerText = FString::Printf(TEXT("%s 팀 슬롯 %d - %s%s"),
                IsRedTeamSlot(SlotIndex) ? TEXT("Red") : TEXT("Blue"),
                GetTeamSlotIndex(SlotIndex),
                *Player.PlayerName,
                Player.IsHost ? TEXT(" (방장)") : TEXT(""));

            UE_LOG(LogTemp, Log, TEXT("슬롯 %d에 플레이어 설정: %s"), SlotIndex, *PlayerText);
            PlayerNameTexts[SlotIndex]->SetText(FText::FromString(PlayerText));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("유효하지 않은 슬롯 인덱스: %d (TextBlocks: %d)"),
                SlotIndex, PlayerNameTexts.Num());
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
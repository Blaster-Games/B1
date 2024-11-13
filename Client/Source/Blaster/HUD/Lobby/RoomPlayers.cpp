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

void URoomPlayers::UpdatePlayers(const TArray<FPlayerInfo>& Players)
{
    if (PlayerNameTexts.Num() == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("RoomPlayers: Late initialization"));
        InitializePlayerTextBlocks();
    }

    UE_LOG(LogTemp, Log, TEXT("RoomPlayers: Updating players list with %d players"), Players.Num());

    // 모든 TextBlock 초기화
    for (int32 i = 0; i < PlayerNameTexts.Num(); ++i)
    {
        if (PlayerNameTexts[i])
        {
            PlayerNameTexts[i]->SetText(FText::FromString(TEXT("")));
            UE_LOG(LogTemp, Log, TEXT("RoomPlayers: Cleared text for player slot %d"), i + 1);
        }
    }

    // 플레이어 정보 설정
    for (int32 i = 0; i < Players.Num() && i < PlayerNameTexts.Num(); ++i)
    {
        if (PlayerNameTexts[i])
        {
            FString PlayerText = Players[i].PlayerName;
            if (Players[i].IsHost)
            {
                PlayerText += TEXT(" (방장)");
            }
            PlayerNameTexts[i]->SetText(FText::FromString(PlayerText));
            UE_LOG(LogTemp, Log, TEXT("RoomPlayers: Set player %d name to: %s"), i + 1, *PlayerText);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("RoomPlayers: Cannot set player %d name - TextBlock is null"), i + 1);
        }
    }
}
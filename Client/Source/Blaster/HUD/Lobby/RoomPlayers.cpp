
#include "RoomPlayers.h"

void URoomPlayers::NativeConstruct()
{
    Super::NativeConstruct();
    InitializePlayerTextBlocks();
}

void URoomPlayers::InitializePlayerTextBlocks()
{
    // TextBlock 포인터들을 배열에 추가
    PlayerNameTexts.Add(Player1NameText);
    PlayerNameTexts.Add(Player2NameText);
    PlayerNameTexts.Add(Player3NameText);
    PlayerNameTexts.Add(Player4NameText);
    PlayerNameTexts.Add(Player5NameText);
    PlayerNameTexts.Add(Player6NameText);
    PlayerNameTexts.Add(Player7NameText);
    PlayerNameTexts.Add(Player8NameText);
}

void URoomPlayers::UpdatePlayers(const TArray<FPlayerInfo>& Players)
{
    // 모든 TextBlock 초기화
    for (UTextBlock* TextBlock : PlayerNameTexts)
    {
        if (TextBlock)
        {
            TextBlock->SetText(FText::FromString(TEXT("")));
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
        }
    }
}
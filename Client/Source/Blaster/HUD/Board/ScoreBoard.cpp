


#include "ScoreBoard.h"
#include "Blaster/HUD/Board/PlayerScore.h"
#include "Components/VerticalBox.h"

void UScoreBoard::UpdateScoreboard(const TArray<FPlayerScoreData>& PlayerScoreData)
{
    if (!PlayerList) return;

    // 리스트 초기화
    PlayerList->ClearChildren();

    // 데이터 정렬을 위한 복사
    TArray<FPlayerScoreData> SortedData = PlayerScoreData;
    SortPlayersByKills(SortedData);

    // UI 업데이트
    for (const FPlayerScoreData& PlayerData : SortedData)
    {
        if (PlayerScoreClass)
        {
            if (UPlayerScore* PlayerScore = CreateWidget<UPlayerScore>(this, PlayerScoreClass))
            {
                PlayerScore->SetPlayerData(PlayerData);
                PlayerList->AddChild(PlayerScore);
            }
        }
    }
}

void UScoreBoard::SortPlayersByKills(TArray<FPlayerScoreData>& PlayerScoreData)
{
    PlayerScoreData.Sort([](const FPlayerScoreData& A, const FPlayerScoreData& B) {
        return A.Kill > B.Kill;  // 킬 수가 많은 순으로 정렬
        });
}




#include "HUD/Board/TeamScoreBoard.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Blaster/HUD/Board/TeamPlayerScore.h"

void UTeamScoreBoard::UpdateTeamScoreboard(const TArray<FTeamPlayerScoreData>& TeamPlayerScoreData, float RedScore, float BlueScore)
{
    if (!RedTeamList || !BlueTeamList) return;

    // 팀 점수 업데이트
    if (RedTeamScore)
    {
        RedTeamScore->SetText(FText::AsNumber(FMath::RoundToInt(RedScore)));
    }
    if (BlueTeamScore)
    {
        BlueTeamScore->SetText(FText::AsNumber(FMath::RoundToInt(BlueScore)));
    }

    // 리스트 초기화
    RedTeamList->ClearChildren();
    BlueTeamList->ClearChildren();

    // 데이터 복사 및 정렬
    TArray<FTeamPlayerScoreData> SortedData = TeamPlayerScoreData;
    SortPlayersByKills(SortedData);

    // 팀별로 플레이어 위젯 생성 및 추가
    for (const FTeamPlayerScoreData& PlayerData : SortedData)
    {
        if (TeamPlayerScoreClass)
        {
            UTeamPlayerScore* PlayerScoreWidget = CreateWidget<UTeamPlayerScore>(this, TeamPlayerScoreClass);
            if (PlayerScoreWidget)
            {
                PlayerScoreWidget->SetPlayerData(PlayerData);

                // 팀에 따라 적절한 리스트에 추가
                if (PlayerData.Team == ETeam::ET_RedTeam)
                {
                    RedTeamList->AddChild(PlayerScoreWidget);
                }
                else if (PlayerData.Team == ETeam::ET_BlueTeam)
                {
                    BlueTeamList->AddChild(PlayerScoreWidget);
                }
            }
        }
    }
}

void UTeamScoreBoard::SortPlayersByKills(TArray<FTeamPlayerScoreData>& TeamPlayerScoreData)
{
    TeamPlayerScoreData.Sort([](const FTeamPlayerScoreData& A, const FTeamPlayerScoreData& B) {
        return A.Kill > B.Kill;  // 킬 수가 많은 순으로 정렬
        });
}

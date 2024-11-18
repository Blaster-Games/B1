


#include "HUD/Board/TeamPlayerScore.h"
#include "Components/TextBlock.h"

void UTeamPlayerScore::NativeConstruct()
{
    Super::NativeConstruct();

}

void UTeamPlayerScore::SetPlayerData(const FTeamPlayerScoreData& Data)
{
    if (NameText)
    {
        // 이름이 10글자를 넘으면 잘라내고 ... 추가
        FString DisplayName = Data.PlayerName;
        if (DisplayName.Len() > 10)
        {
            DisplayName = DisplayName.Left(10) + TEXT("...");
        }
        NameText->SetText(FText::FromString(DisplayName));
    }
    if (KillText)
    {
        KillText->SetText(FText::AsNumber(Data.Kill));
    }
    if (DeathText)
    {
        DeathText->SetText(FText::AsNumber(Data.Death));
    }
    if (CoinText)
    {
        CoinText->SetText(FText::AsNumber(Data.Coin));
    }
}

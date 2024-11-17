


#include "HUD/Board/PlayerScore.h"
#include "Components/TextBlock.h"

void UPlayerScore::NativeConstruct()
{
    Super::NativeConstruct();
}

void UPlayerScore::SetPlayerData(const FPlayerScoreData& Data)
{
    if (NameText)
    {
        NameText->SetText(FText::FromString(Data.PlayerName));
    }

    if (KillText)
    {
        KillText->SetText(FText::AsNumber(Data.Kill));
    }

    if (DeathText)
    {
        DeathText->SetText(FText::AsNumber(Data.Death));
    }
}

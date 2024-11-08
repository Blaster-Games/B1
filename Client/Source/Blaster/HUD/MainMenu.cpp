

#include "GameInstance/BlasterGameInstance.h"
#include "MainMenu.h"

void UMainMenu::NativeConstruct()
{
    Super::NativeConstruct();

    if (EnterLobbyButton)
    {
        EnterLobbyButton->OnClicked.AddDynamic(this, &UMainMenu::OnEnterLobbyButtonClicked);
    }
}

void UMainMenu::OnEnterLobbyButtonClicked()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        //if (UBlasterWebSubsystem* WebSubsystem = GameInstance->GetSubsystem<UBlasterWebSubsystem>())
        //{
        //}
    }
}
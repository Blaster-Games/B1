#include "MainMenu.h"
#include "GameInstance/BlasterNetworkSubsystem.h"
#include "GameInstance/BlasterGameInstance.h"

void UMainMenu::NativeConstruct()
{
    Super::NativeConstruct();
    if (EnterLobbyButton)
    {
        EnterLobbyButton->OnClicked.AddDynamic(this, &UMainMenu::OnEnterLobbyButtonClicked);
    }
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            // HandleEnterLobbyResponse 함수를 바인딩
            NetworkSubsystem->OnEnterLobbyResponse.AddDynamic(this, &UMainMenu::ShowLobbyWidget);
        }
    }
}

void UMainMenu::OnEnterLobbyButtonClicked()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            NetworkSubsystem->SendEnterLobbyReq();
        }
    }
}

void UMainMenu::ShowLobbyWidget()
{
    RemoveFromParent();

    if (LobbyWidgetClass)
    {
        UUserWidget* LobbyWidget = CreateWidget<UUserWidget>(GetWorld(), LobbyWidgetClass);
        if (LobbyWidget)
        {
            LobbyWidget->AddToViewport();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to Create LobbyWidget"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("LobbyWidgetClass is not set!"));
    }
}
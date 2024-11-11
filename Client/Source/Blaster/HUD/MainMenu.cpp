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
            NetworkSubsystem->OnEnterLobbyResponse.AddDynamic(this, &UMainMenu::HandleEnterLobbyResponse);
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

void UMainMenu::HandleEnterLobbyResponse()
{
    UE_LOG(LogTemp, Warning, TEXT("HandleEnterLobbyResponse Called"));

    RemoveFromParent();
    UE_LOG(LogTemp, Warning, TEXT("MainMenu Removed"));

    if (LobbyWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("LobbyWidgetClass exists: %s"), *LobbyWidgetClass->GetName());

        UUserWidget* LobbyWidget = CreateWidget<UUserWidget>(GetWorld(), LobbyWidgetClass);
        if (LobbyWidget)
        {
            UE_LOG(LogTemp, Warning, TEXT("LobbyWidget Created Successfully"));
            LobbyWidget->AddToViewport();
            UE_LOG(LogTemp, Warning, TEXT("LobbyWidget Added to Viewport"));
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
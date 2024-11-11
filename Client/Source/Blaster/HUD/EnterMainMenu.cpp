// EnterMainMenu.cpp
#include "EnterMainMenu.h"
#include "GameInstance/BlasterGameInstance.h"
#include "GameInstance/BlasterWebSubsystem.h"
#include "GameInstance/BlasterNetworkSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UEnterMainMenu::NativeConstruct()
{
    Super::NativeConstruct();

    if (EnterButton)
    {
        EnterButton->OnClicked.AddDynamic(this, &UEnterMainMenu::OnEnterButtonClicked);
    }

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            NetworkSubsystem->OnAuthSuccess.AddDynamic(this, &UEnterMainMenu::HandleEnterSuccess);
            NetworkSubsystem->OnAuthFailed.AddDynamic(this, &UEnterMainMenu::HandleEnterFailed);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[EnterMainMenu] Failed to get NetworkSubsystem"));
        }
    }
}

void UEnterMainMenu::OnEnterButtonClicked()
{
    UE_LOG(LogTemp, Log, TEXT("[EnterMainMenu] Enter button clicked"));

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterNetworkSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UBlasterNetworkSubsystem>())
        {
            // 연결 상태 확인 (Socket이나 Session이 유효한지)
            if (NetworkSubsystem->Socket && NetworkSubsystem->GameServerSession)
            {
                NetworkSubsystem->SendAuthReq();
                UE_LOG(LogTemp, Log, TEXT("[EnterMainMenu] Sending auth request"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("[EnterMainMenu] Not connected to server"));
                HandleEnterFailed(TEXT("서버에 연결되어 있지 않습니다."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[EnterMainMenu] Failed to get NetworkSubsystem"));
        }
    }
}

void UEnterMainMenu::HandleEnterSuccess()
{
    RemoveFromParent();

    if (UClass* WidgetClass = MainMenuWidgetClass.LoadSynchronous())
    {
        UUserWidget* EnterWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
        if (EnterWidget)
        {
            EnterWidget->AddToViewport();
        }
    }
}

void UEnterMainMenu::HandleEnterFailed(const FString& ErrorMessage)
{
    if (EnterMainMenuText)
    {
        EnterMainMenuText->SetText(FText::FromString(ErrorMessage));
    }
}
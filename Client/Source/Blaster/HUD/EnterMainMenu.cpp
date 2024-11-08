// EnterMainMenu.cpp
#include "EnterMainMenu.h"
#include "GameInstance/BlasterGameInstance.h"
#include "GameInstance/BlasterWebSubsystem.h"
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
        if (UBlasterWebSubsystem* WebSubsystem = GameInstance->GetSubsystem<UBlasterWebSubsystem>())
        {
            // 웹 서브시스템에 적절한 델리게이트 추가 필요
            //WebSubsystem->OnEnterSuccessDelegate.AddDynamic(this, &UEnterMainMenu::HandleEnterSuccess);
            //WebSubsystem->OnEnterFailedDelegate.AddDynamic(this, &UEnterMainMenu::HandleEnterFailed);
        }
    }
}

void UEnterMainMenu::OnEnterButtonClicked()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterWebSubsystem* WebSubsystem = GameInstance->GetSubsystem<UBlasterWebSubsystem>())
        {
            //WebSubsystem->RequestEnter();  // 웹 서브시스템에 적절한 요청 메서드 추가 필요
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
    // 실패 처리
    if (EnterMainMenuText)
    {
        EnterMainMenuText->SetText(FText::FromString(ErrorMessage));
    }
}
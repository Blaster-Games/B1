// Login.cpp
#include "Login.h"
#include "GameInstance/BlasterGameInstance.h"
#include "GameInstance/BlasterWebSubsystem.h"
#include "Kismet/GameplayStatics.h"

void ULogin::NativeConstruct()
{
    Super::NativeConstruct();

    if (LoginButton)
    {
        LoginButton->OnClicked.AddDynamic(this, &ULogin::OnLoginButtonClicked);
    }

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UBlasterWebSubsystem* WebSubsystem = GameInstance->GetSubsystem<UBlasterWebSubsystem>())
        {
            WebSubsystem->OnLoginSuccessDelegate.AddDynamic(this, &ULogin::HandleLoginSuccess);
            WebSubsystem->OnLoginFailedDelegate.AddDynamic(this, &ULogin::HandleLoginFailed);
        }
    }
}

void ULogin::OnLoginButtonClicked()
{
    if (EmailTextBox && PasswordTextBox)
    {
        FString Email = EmailTextBox->GetText().ToString();
        FString Password = PasswordTextBox->GetText().ToString();

        // 게임 인스턴스를 통해 웹 서브시스템 접근
        if (UGameInstance* GameInstance = GetGameInstance())
        {
            if (UBlasterWebSubsystem* WebSubsystem = GameInstance->GetSubsystem<UBlasterWebSubsystem>())
            {
                WebSubsystem->RequestLogin(Email, Password);
            }
        }
    }
}

void ULogin::HandleLoginSuccess()
{
    UE_LOG(LogTemp, Warning, TEXT("[Login] HandleLoginSuccess called"));

    RemoveFromParent();
    UE_LOG(LogTemp, Log, TEXT("[Login] Current widget removed from parent"));

    if (UClass* WidgetClass = EnterMainMenuWidgetClass.LoadSynchronous())
    {
        UE_LOG(LogTemp, Log, TEXT("[Login] Enter widget class loaded successfully"));

        UUserWidget* EnterWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
        if (EnterWidget)
        {
            UE_LOG(LogTemp, Log, TEXT("[Login] Enter widget created successfully"));
            EnterWidget->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("[Login] Enter widget added to viewport"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[Login] Failed to create Enter widget"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Login] Failed to load Enter widget class: %s"),
            *EnterMainMenuWidgetClass.ToString());
    }
}

void ULogin::HandleLoginFailed(const FString& ErrorMessage)
{
    // 로그인 실패 시 수행할 동작
    // 예: 에러 메시지 표시
    // ErrorTextBlock->SetText(FText::FromString(ErrorMessage));
}
#include "OutGamePlayerController.h"
#include "GameInstance/BlasterGameInstance.h"

AOutGamePlayerController::AOutGamePlayerController()
{
    // 생성자에서 로그 추가
    UE_LOG(LogTemp, Log, TEXT("OutGamePlayerController Constructor called"));
}

void AOutGamePlayerController::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("OutGamePlayerController BeginPlay - Starting UI Setup"));

    // GameInstance 상태 로깅
    if (UBlasterGameInstance* GI = Cast<UBlasterGameInstance>(GetGameInstance()))
    {
        UE_LOG(LogTemp, Log, TEXT("GameInstance found - ShouldShowRoom: %s"),
            GI->GetShouldShowRoom() ? TEXT("True") : TEXT("False"));
    }

    SetupUIInputMode();
    ShowLobbyWidget();

    UE_LOG(LogTemp, Log, TEXT("OutGamePlayerController BeginPlay - Setup Complete"));
}

void AOutGamePlayerController::ShowLobbyWidget()
{
    UE_LOG(LogTemp, Log, TEXT("ShowLobbyWidget - Starting widget creation process"));

    UBlasterGameInstance* GI = Cast<UBlasterGameInstance>(GetGameInstance());
    if (!GI)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get GameInstance in ShowLobbyWidget"));
        return;
    }

    if (!IsLocalController())
    {
        UE_LOG(LogTemp, Warning, TEXT("ShowLobbyWidget - Not a local controller, returning"));
        return;
    }

    if (!GI->GetShouldShowRoom())
    {
        UE_LOG(LogTemp, Warning, TEXT("ShowLobbyWidget - ShouldShowRoom is false, not showing lobby widget"));
        return;
    }

    if (!LobbyWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ShowLobbyWidget - LobbyWidgetClass is not set!"));
        return;
    }

    if (CurrentLobbyWidget)
    {
        UE_LOG(LogTemp, Log, TEXT("ShowLobbyWidget - Removing existing widget"));
        CurrentLobbyWidget->RemoveFromParent();
        CurrentLobbyWidget = nullptr;
    }

    CurrentLobbyWidget = CreateWidget<ULobby>(this, LobbyWidgetClass);
    if (CurrentLobbyWidget)
    {
        UE_LOG(LogTemp, Log, TEXT("ShowLobbyWidget - Widget created successfully"));

        CurrentLobbyWidget->AddToViewport();
        UE_LOG(LogTemp, Log, TEXT("ShowLobbyWidget - Widget added to viewport"));

        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(InputMode);
        SetShowMouseCursor(true);
        UE_LOG(LogTemp, Log, TEXT("ShowLobbyWidget - UI input mode set"));

        GI->SetShowRoomFlag(false);
        UE_LOG(LogTemp, Log, TEXT("ShowLobbyWidget - Room flag set to false"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ShowLobbyWidget - Failed to Create LobbyWidget"));
    }
}

void AOutGamePlayerController::SetupUIInputMode()
{
    UE_LOG(LogTemp, Log, TEXT("SetupUIInputMode - Setting up UI input mode"));

    FInputModeUIOnly InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
    SetShowMouseCursor(true);

    UE_LOG(LogTemp, Log, TEXT("SetupUIInputMode - UI input mode setup complete"));
}
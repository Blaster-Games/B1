#include "OutGamePlayerController.h"

AOutGamePlayerController::AOutGamePlayerController()
{
    // 생성자 코드
}

void AOutGamePlayerController::BeginPlay()
{
    Super::BeginPlay();
    SetupUIInputMode();
}

void AOutGamePlayerController::SetupUIInputMode()
{
    FInputModeUIOnly InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
    SetShowMouseCursor(true);
}

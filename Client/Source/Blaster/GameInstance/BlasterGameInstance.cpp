#include "BlasterGameInstance.h"
#include "GameInstance/BlasterNetworkSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/Save/BlasterSaveSettings.h"
#include "Blaster/Character/MyBlasterCharacter.h"

void UBlasterGameInstance::Init()
{
    Super::Init();
    NetworkSystem = GetSubsystem<UBlasterNetworkSubsystem>();
    LoadSettings();
}

void UBlasterGameInstance::ConnectToServer()
{
    if (NetworkSystem)
    {
        NetworkSystem->ConnectToGameServer();
    }
}

void UBlasterGameInstance::DisconnectFromServer()
{
    if (NetworkSystem)
    {
        NetworkSystem->DisconnectFromGameServer();
    }
}

void UBlasterGameInstance::HandleNetworkPackets()
{
    if (NetworkSystem)
    {
        NetworkSystem->HandleRecvPackets();
    }
}

void UBlasterGameInstance::SendNetworkPacket(SendBufferRef SendBuffer)
{
    if (NetworkSystem)
    {
        NetworkSystem->SendPacket(SendBuffer);
    }
}

void UBlasterGameInstance::SetSensitivitySettings(const FSensitivitySettings& NewSettings)
{
    SensitivitySettings = NewSettings;

    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            if (AMyBlasterCharacter* Character = Cast<AMyBlasterCharacter>(PC->GetPawn()))
            {
                Character->UpdateSensitivityMultiplier();
            }
        }
    }
}

void UBlasterGameInstance::SaveSettings()
{
    UBlasterSaveSettings* SaveGameInstance = Cast<UBlasterSaveSettings>(UGameplayStatics::CreateSaveGameObject(UBlasterSaveSettings::StaticClass()));
    if (SaveGameInstance)
    {
        SaveGameInstance->SensitivitySettings = SensitivitySettings;
        UGameplayStatics::SaveGameToSlot(SaveGameInstance, UBlasterSaveSettings::SaveSlotName, UBlasterSaveSettings::UserIndex);
    }
}

void UBlasterGameInstance::LoadSettings()
{
    UBlasterSaveSettings* LoadedGame = Cast<UBlasterSaveSettings>(
        UGameplayStatics::LoadGameFromSlot(UBlasterSaveSettings::SaveSlotName, UBlasterSaveSettings::UserIndex)
    );

    if (LoadedGame)
    {
        SensitivitySettings = LoadedGame->SensitivitySettings;
    }
    else
    {
        // 저장된 설정이 없는 경우 기본값 사용
        SensitivitySettings.Sensitivity = 6.0f;
        SensitivitySettings.AimSensitivity = 6.0f;
        SensitivitySettings.ScopedSensitivity = 6.0f;
    }
}




#include "Setting.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"
#include "Components/Button.h"
#include "Blaster/GameInstance/BlasterGameInstance.h"
#include "Blaster/HUD/ReturnToMainMenu.h"

void USetting::MenuTearDown()
{
    // 슬라이더 & 스핀박스 바인딩 해제
    if (SensSlider && SensSpinBox)
    {
        SensSlider->OnValueChanged.RemoveDynamic(this, &USetting::OnSensSliderValueChanged);
        SensSpinBox->OnValueChanged.RemoveDynamic(this, &USetting::OnSensSpinBoxValueChanged);
    }

    if (AimSensSlider && AimSensSpinBox)
    {
        AimSensSlider->OnValueChanged.RemoveDynamic(this, &USetting::OnAimSensSliderValueChanged);
        AimSensSpinBox->OnValueChanged.RemoveDynamic(this, &USetting::OnAimSensSpinBoxValueChanged);
    }

    if (ScopedSensSlider && ScopedSensSpinBox)
    {
        ScopedSensSlider->OnValueChanged.RemoveDynamic(this, &USetting::OnScopedSensSliderValueChanged);
        ScopedSensSpinBox->OnValueChanged.RemoveDynamic(this, &USetting::OnScopedSensSpinBoxValueChanged);
    }

    // 버튼 바인딩 해제
    if (SaveButton)
    {
        SaveButton->OnClicked.RemoveDynamic(this, &USetting::OnSaveButtonClicked);
    }
    if (CancelButton)
    {
        CancelButton->OnClicked.RemoveDynamic(this, &USetting::OnCancelButtonClicked);
    }

    RemoveFromParent();
}

void USetting::NativeConstruct()
{

    Super::NativeConstruct();

    if (SensSlider && SensSpinBox)
    {
        SensSlider->OnValueChanged.AddDynamic(this, &USetting::OnSensSliderValueChanged);
        SensSpinBox->OnValueChanged.AddDynamic(this, &USetting::OnSensSpinBoxValueChanged);
    }
    
    if (AimSensSlider && AimSensSpinBox)
    {
        AimSensSlider->OnValueChanged.AddDynamic(this, &USetting::OnAimSensSliderValueChanged);
        AimSensSpinBox->OnValueChanged.AddDynamic(this, &USetting::OnAimSensSpinBoxValueChanged);
    }

    if (ScopedSensSlider && ScopedSensSpinBox)
    {
        ScopedSensSlider->OnValueChanged.AddDynamic(this, &USetting::OnScopedSensSliderValueChanged);
        ScopedSensSpinBox->OnValueChanged.AddDynamic(this, &USetting::OnScopedSensSpinBoxValueChanged);
    }

    if (SaveButton)
    {
        SaveButton->OnClicked.AddDynamic(this, &USetting::OnSaveButtonClicked);
    }
    if (CancelButton)
    {
        CancelButton->OnClicked.AddDynamic(this, &USetting::OnCancelButtonClicked);
    }

    LoadCurrentSettings();
}

void USetting::OnSensSliderValueChanged(float Value)
{
	if (SensSpinBox && !FMath::IsNearlyEqual(SensSpinBox->GetValue(), Value))
	{
		SensSpinBox->SetValue(Value);
	}
}

void USetting::OnSensSpinBoxValueChanged(float Value)
{
	if (SensSlider && !FMath::IsNearlyEqual(SensSlider->GetValue(), Value))
	{
		SensSlider->SetValue(Value);
	}
}

void USetting::OnAimSensSliderValueChanged(float Value)
{
    if (AimSensSpinBox && !FMath::IsNearlyEqual(AimSensSpinBox->GetValue(), Value))
    {
        AimSensSpinBox->SetValue(Value);
    }
}

void USetting::OnAimSensSpinBoxValueChanged(float Value)
{
    if (AimSensSlider && !FMath::IsNearlyEqual(AimSensSlider->GetValue(), Value))
    {
        AimSensSlider->SetValue(Value);
    }
}

void USetting::OnScopedSensSliderValueChanged(float Value)
{
    if (ScopedSensSpinBox && !FMath::IsNearlyEqual(ScopedSensSpinBox->GetValue(), Value))
    {
        ScopedSensSpinBox->SetValue(Value);
    }
}

void USetting::OnScopedSensSpinBoxValueChanged(float Value)
{
    if (ScopedSensSlider && !FMath::IsNearlyEqual(ScopedSensSlider->GetValue(), Value))
    {
        ScopedSensSlider->SetValue(Value);
    }
}

void USetting::OnSaveButtonClicked()
{
    if (UBlasterGameInstance* GameInstance = Cast<UBlasterGameInstance>(GetGameInstance()))
    {
        FSensitivitySettings NewSettings;
        NewSettings.Sensitivity = SensSlider->GetValue();
        NewSettings.AimSensitivity = AimSensSlider->GetValue();
        NewSettings.ScopedSensitivity = ScopedSensSlider->GetValue();

        GameInstance->SetSensitivitySettings(NewSettings);
        GameInstance->SaveSettings();
    }

    MenuTearDown();

    if (ParentWidget)
    {
        ParentWidget->MenuTearDown();  // ReturnToMainMenu 메뉴 닫기
    }
}

void USetting::OnCancelButtonClicked()
{
    // 초기값으로 되돌리기
    if (SensSlider) SensSlider->SetValue(InitialSettings.Sensitivity);
    if (AimSensSlider) AimSensSlider->SetValue(InitialSettings.AimSensitivity);
    if (ScopedSensSlider) ScopedSensSlider->SetValue(InitialSettings.ScopedSensitivity);

    MenuTearDown();

    if (ParentWidget)
    {
        ParentWidget->MenuTearDown();  // ReturnToMainMenu 메뉴 닫기
        // 이렇게 한 이유 -> 입력 모드 설정을 여러번 하지 않으려고!!
    }
}

void USetting::LoadCurrentSettings()
{
    if (UBlasterGameInstance* GameInstance = Cast<UBlasterGameInstance>(GetGameInstance()))
    {
        InitialSettings = GameInstance->GetSensitivitySettings();

        // UI에 현재 값 설정
        if (SensSlider) SensSlider->SetValue(InitialSettings.Sensitivity);
        if (SensSpinBox) SensSpinBox->SetValue(InitialSettings.Sensitivity);

        if (AimSensSlider) AimSensSlider->SetValue(InitialSettings.AimSensitivity);
        if (AimSensSpinBox) AimSensSpinBox->SetValue(InitialSettings.AimSensitivity);

        if (ScopedSensSlider) ScopedSensSlider->SetValue(InitialSettings.ScopedSensitivity);
        if (ScopedSensSpinBox) ScopedSensSpinBox->SetValue(InitialSettings.ScopedSensitivity);
    }
}





#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/Save/BlasterSaveSettings.h"
#include "Setting.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API USetting : public UUserWidget
{
	GENERATED_BODY()


public:
    //void MenuSetup(); // 이거 없앤 이유 -> 입력 모드 처리를 따로 하지 않고 기존의 ReturnToMainMenu를 이용할거라서.
    void MenuTearDown();

    void SetParentWidget(class UReturnToMainMenu* Widget) { ParentWidget = Widget; }
private:
    virtual void NativeConstruct() override;

    UPROPERTY()
    class UReturnToMainMenu* ParentWidget;

    UPROPERTY(meta = (BindWidget))
    class USlider* SensSlider;

    UPROPERTY(meta = (BindWidget))
    class USpinBox* SensSpinBox;

    UPROPERTY(meta = (BindWidget))
    USlider* AimSensSlider;

    UPROPERTY(meta = (BindWidget))
    USpinBox* AimSensSpinBox;

    UPROPERTY(meta = (BindWidget))
    USlider* ScopedSensSlider;

    UPROPERTY(meta = (BindWidget))
    USpinBox* ScopedSensSpinBox;

    UPROPERTY(meta = (BindWidget))
    class UButton* SaveButton;
    UPROPERTY(meta = (BindWidget))
    UButton* CancelButton;

    UFUNCTION()
    void OnSensSliderValueChanged(float Value);

    UFUNCTION()
    void OnSensSpinBoxValueChanged(float Value);

    UFUNCTION()
    void OnAimSensSliderValueChanged(float Value);

    UFUNCTION()
    void OnAimSensSpinBoxValueChanged(float Value);

    UFUNCTION()
    void OnScopedSensSliderValueChanged(float Value);

    UFUNCTION()
    void OnScopedSensSpinBoxValueChanged(float Value);

    UFUNCTION()
    void OnSaveButtonClicked();

    UFUNCTION()
    void OnCancelButtonClicked();

    // 초기 설정값 저장용
    FSensitivitySettings InitialSettings;

    void LoadCurrentSettings();

    UPROPERTY()
    class APlayerController* PlayerController;
};

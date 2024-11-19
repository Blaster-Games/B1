#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomPlayers.h"
#include "Components/Button.h"
#include "../RoomTypes.h"
#include "Components/TextBlock.h"
#include "RoomDetail.generated.h"

class UBlasterNetworkSubsystem;

UCLASS()
class BLASTER_API URoomDetail : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION()
    void UpdateRoomInfo(const FRoomDetailInfo& RoomInfo);

	UFUNCTION()
	void TravelToHostServer(const FString& HostAddress, int32 HostPort);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:

    // UI 컴포넌트들
    UPROPERTY(meta = (BindWidget))
    URoomPlayers* RoomPlayers;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RoomNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RoomTypeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PlayerCountText;

    UPROPERTY(meta = (BindWidget))
    class UButton* RedTeamButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* BlueTeamButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* StartGameButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* LeaveGameButton;

    // 서브시스템 참조
    UPROPERTY()
    mutable UBlasterNetworkSubsystem* NetworkSubsystem;
    UBlasterNetworkSubsystem* GetNetworkSubsystem() const;

    UFUNCTION()
    void OnRedTeamButtonClicked();

    UFUNCTION()
    void OnBlueTeamButtonClicked();

    UFUNCTION()
    void RequestTeamChange();

	UFUNCTION()
	void OnStartGameButtonClicked();

	UFUNCTION()
	void OnLeaveGameButtonClicked();

private:
    void UpdateUI();
    FString GetRoomTypeString(EGameMode RoomType) const;
};
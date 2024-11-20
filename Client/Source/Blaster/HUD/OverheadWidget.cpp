

#include "HUD/OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/BlasterPlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	if (!InPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("InPawn is null"));
		SetDisplayText(TEXT("Unknown"));
		return;
	}

	// 디버그를 위한 로그 추가
	UE_LOG(LogTemp, Log, TEXT("ShowPlayerNetRole called for Pawn: %s"), *InPawn->GetName());

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this, InPawn]()
		{
			if (!InPawn)
			{
				UE_LOG(LogTemp, Warning, TEXT("InPawn became null"));
				SetDisplayText(TEXT("Unknown"));
				return;
			}

			ABlasterPlayerState* PlayerState = InPawn->GetPlayerState<ABlasterPlayerState>();
			if (PlayerState)
			{
				FString Nickname = PlayerState->GetNickname();
				UE_LOG(LogTemp, Log, TEXT("Found PlayerState with nickname: %s"), *Nickname);
				SetDisplayText(Nickname);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("PlayerState is null for Pawn: %s"), *InPawn->GetName());
				SetDisplayText(TEXT("Unknown"));
			}
		},
		0.5f,
		false
	);
}

void UOverheadWidget::UpdateVisibility(APawn* LocalPawn, APawn* TargetPawn)
{
	if (!LocalPawn || !TargetPawn || !DisplayText)
	{
		return;
	}

	// 거리 계산
	const float MaxVisibleDistance = 2500.f; // 25미터
	const float DistanceSquared = FVector::DistSquared(LocalPawn->GetActorLocation(), TargetPawn->GetActorLocation());

	// 최대 거리보다 멀면 바로 숨김
	if (DistanceSquared > (MaxVisibleDistance * MaxVisibleDistance))
	{
		DisplayText->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	// 카메라 위치 기준으로 체크
	APlayerController* LocalController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!LocalController || !LocalController->PlayerCameraManager)
	{
		return;
	}

	const FVector CameraLocation = LocalController->PlayerCameraManager->GetCameraLocation();
	const FVector TargetLocation = TargetPawn->GetActorLocation() + FVector(0, 0, 50); // 머리 위치

	// 라인 트레이스로 시야 체크
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(LocalPawn);
	QueryParams.AddIgnoredActor(TargetPawn);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraLocation,
		TargetLocation,
		ECC_Visibility,
		QueryParams
	);

	// 벽에 가려졌는지 확인
	if (bHit && HitResult.GetActor() != TargetPawn)
	{
		DisplayText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		DisplayText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}




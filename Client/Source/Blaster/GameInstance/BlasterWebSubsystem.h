#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Http.h"
#include "BlasterWebSubsystem.generated.h"

UCLASS()
class BLASTER_API UBlasterWebSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	// 초기화/종료
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// HTTP 요청 메서드
	void RequestLogin(const FString& Username, const FString& Password);

	// 인증 관련 접근자
	bool IsAuthenticated() const { return !AccessToken.IsEmpty(); }
	const FString& GetAccessToken() const { return AccessToken; }
	const FString& GetRefreshToken() const { return RefreshToken; }
	const FString& GetNickname() const { return Nickname; }
	int32 GetUserId() const { return UserId; }

	// 로그인 결과 알림을 위한 델리게이트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoginSuccessDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginFailedDelegate, const FString&, ErrorMessage);

	UPROPERTY(BlueprintAssignable, Category = "Web|Auth")
	FOnLoginSuccessDelegate OnLoginSuccessDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Web|Auth")
	FOnLoginFailedDelegate OnLoginFailedDelegate;

private:
	// API endpoint
	const FString LOGIN_ENDPOINT = TEXT("/api/member/login");

	// Response handlers
	void OnLoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);

	// 로그인 결과 처리
	void OnLoginSuccess();
	void OnLoginFailed(const FString& ErrorMessage = TEXT(""));

	// 헬퍼 함수
	void ClearAuthData();

private:
	FHttpModule* HttpModule;
	FString BaseUrl = TEXT("https://5274-112-217-167-202.ngrok-free.app");

	// 인증 및 사용자 데이터
	FString AccessToken;
	FString RefreshToken;
	FString Nickname;
	int32 UserId;

	// JSON 필드 상수
	const FString FIELD_NICKNAME = TEXT("nickname");
	const FString FIELD_ID = TEXT("id");
	const FString FIELD_ACCESS_TOKEN = TEXT("accessToken");
	const FString FIELD_REFRESH_TOKEN = TEXT("refreshToken");

	// 헤더 상수
	const FString AUTH_HEADER = TEXT("Authorization");
	const FString BEARER_PREFIX = TEXT("Bearer ");
	const FString CONTENT_TYPE_HEADER = TEXT("Content-Type");
	const FString FORM_URLENCODED = TEXT("application/x-www-form-urlencoded");
};
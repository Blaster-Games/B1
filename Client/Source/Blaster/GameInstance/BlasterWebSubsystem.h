// BlasterWebSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Http.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "BlasterWebSubsystem.generated.h"

// Delegate 선언을 클래스 선언 전에 해야 함
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoginSuccessDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginFailedDelegate, const FString&, ErrorMessage);

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

    // 통계 데이터 보내는 메서드
    void SendMatchStats(const class ABlasterGameState* GameState);

    // delegate 프로퍼티
    UPROPERTY(BlueprintAssignable, Category = "Web|Auth")
    FOnLoginSuccessDelegate OnLoginSuccess;

    UPROPERTY(BlueprintAssignable, Category = "Web|Auth")
    FOnLoginFailedDelegate OnLoginFailed;


private:
    // Response handlers
    void OnLoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
    void HandleLoginSuccess();
    void HandleLoginFailed(const FString& ErrorMessage = TEXT(""));

private:
    FHttpModule* HttpModule;
    FString BaseUrl;

    // const 문자열 정의
    static const TCHAR* const LOGIN_ENDPOINT;
    static const TCHAR* const MATCH_STATS_ENDPOINT;
    static const TCHAR* const FIELD_NICKNAME;
    static const TCHAR* const FIELD_ID;
    static const TCHAR* const FIELD_ACCESS_TOKEN;
    static const TCHAR* const FIELD_REFRESH_TOKEN;
    static const TCHAR* const AUTH_HEADER;
    static const TCHAR* const BEARER_PREFIX;
    static const TCHAR* const CONTENT_TYPE_HEADER;
    static const TCHAR* const FORM_URLENCODED;
};
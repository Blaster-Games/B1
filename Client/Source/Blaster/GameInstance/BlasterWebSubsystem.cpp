#include "BlasterWebSubsystem.h"
#include "BlasterGameInstance.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Blaster/GameState/BlasterGameState.h"


const TCHAR* const UBlasterWebSubsystem::LOGIN_ENDPOINT = TEXT("/api/member/login/game");
const TCHAR* const UBlasterWebSubsystem::MATCH_STATS_ENDPOINT = TEXT("/api/game/match/result");

const TCHAR* const UBlasterWebSubsystem::FIELD_NICKNAME = TEXT("nickname");
const TCHAR* const UBlasterWebSubsystem::FIELD_ID = TEXT("id");
const TCHAR* const UBlasterWebSubsystem::FIELD_ACCESS_TOKEN = TEXT("accessToken");
const TCHAR* const UBlasterWebSubsystem::FIELD_REFRESH_TOKEN = TEXT("refreshToken");
const TCHAR* const UBlasterWebSubsystem::AUTH_HEADER = TEXT("Authorization");
const TCHAR* const UBlasterWebSubsystem::BEARER_PREFIX = TEXT("Bearer ");
const TCHAR* const UBlasterWebSubsystem::CONTENT_TYPE_HEADER = TEXT("Content-Type");
const TCHAR* const UBlasterWebSubsystem::FORM_URLENCODED = TEXT("application/x-www-form-urlencoded");

void UBlasterWebSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    HttpModule = &FHttpModule::Get();
    BaseUrl = TEXT("https://native-pika-possibly.ngrok-free.app");
    //BaseUrl = TEXT("http://localhost:8080");
}

void UBlasterWebSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UBlasterWebSubsystem::RequestLogin(const FString& Username, const FString& Password)
{
    auto Request = HttpModule->CreateRequest();
    Request->SetURL(BaseUrl + LOGIN_ENDPOINT);
    Request->SetVerb(TEXT("POST"));

    FString FormData = FString::Printf(
        TEXT("username=%s&password=%s"),
        *FGenericPlatformHttp::UrlEncode(Username),
        *FGenericPlatformHttp::UrlEncode(Password)
    );

    Request->SetHeader(CONTENT_TYPE_HEADER, FORM_URLENCODED);
    Request->SetContentAsString(FormData);

    UE_LOG(LogTemp, Log, TEXT("[WebSubsystem] Sending login request for user: %s"), *Username);

    Request->OnProcessRequestComplete().BindUObject(this, &UBlasterWebSubsystem::OnLoginResponse);
    Request->ProcessRequest();
}

void UBlasterWebSubsystem::SendMatchStats(const ABlasterGameState* GameState, const FOnRequestComplete& OnComplete)
{
    if (!GameState || !HttpModule)
    {
        UE_LOG(LogTemp, Error, TEXT("SendMatchStats: Invalid GameState or HttpModule"));
        return;
    }

    // JSON 객체 생성
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

    // 맵 정보와 게임 모드 추가
    JsonObject->SetStringField("mapName", "Highrise");
    JsonObject->SetStringField("gameMode", "Teams");

    // 구매 통계
    const FGamePurchaseStats& Stats = GameState->GetGamePurchaseStats();

    UE_LOG(LogTemp, Log, TEXT("SendMatchStats: Creating stats JSON..."));

    // 무기 구매 통계
    TArray<TSharedPtr<FJsonValue>> WeaponStats;
    for (const auto& Pair : Stats.WeaponPurchases)
    {
        FString WeaponName = StaticEnum<EWeaponType>()->GetNameStringByValue((int64)Pair.Key);
        UE_LOG(LogTemp, Log, TEXT("Weapon Purchase - Type: %s, Count: %d"), *WeaponName, Pair.Value);

        TSharedPtr<FJsonObject> WeaponObj = MakeShared<FJsonObject>();
        WeaponObj->SetStringField("weaponType", WeaponName);
        WeaponObj->SetNumberField("purchaseCount", Pair.Value);
        WeaponStats.Add(MakeShared<FJsonValueObject>(WeaponObj));
    }
    JsonObject->SetArrayField("weaponPurchases", WeaponStats);

    // 버프 구매 통계
    TArray<TSharedPtr<FJsonValue>> BuffStats;
    for (const auto& Pair : Stats.BuffPurchases)
    {
        FString BuffName = StaticEnum<EBuffType>()->GetNameStringByValue((int64)Pair.Key);
        UE_LOG(LogTemp, Log, TEXT("Buff Purchase - Type: %s, Count: %d"), *BuffName, Pair.Value);

        TSharedPtr<FJsonObject> BuffObj = MakeShared<FJsonObject>();
        BuffObj->SetStringField("buffType", BuffName);
        BuffObj->SetNumberField("purchaseCount", Pair.Value);
        BuffStats.Add(MakeShared<FJsonValueObject>(BuffObj));
    }
    JsonObject->SetArrayField("buffPurchases", BuffStats);

    // 투척무기 구매 통계
    TArray<TSharedPtr<FJsonValue>> ThrowableStats;
    for (const auto& Pair : Stats.ThrowablePurchases)
    {
        FString ThrowableName = StaticEnum<EThrowType>()->GetNameStringByValue((int64)Pair.Key);
        UE_LOG(LogTemp, Log, TEXT("Throwable Purchase - Type: %s, Count: %d"), *ThrowableName, Pair.Value);

        TSharedPtr<FJsonObject> ThrowableObj = MakeShared<FJsonObject>();
        ThrowableObj->SetStringField("throwType", ThrowableName);
        ThrowableObj->SetNumberField("purchaseCount", Pair.Value);
        ThrowableStats.Add(MakeShared<FJsonValueObject>(ThrowableObj));
    }
    JsonObject->SetArrayField("throwablePurchases", ThrowableStats);

    // JSON 문자열로 변환
    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    UE_LOG(LogTemp, Log, TEXT("SendMatchStats: Final JSON payload: %s"), *JsonString);

    // HTTP 요청 생성
    auto Request = HttpModule->CreateRequest();
    Request->SetURL(BaseUrl + MATCH_STATS_ENDPOINT);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(CONTENT_TYPE_HEADER, TEXT("application/json"));

    // 인증 토큰 추가
    if (UBlasterGameInstance* BlasterGameInstance = Cast<UBlasterGameInstance>(GetGameInstance()))
    {
        const FString AuthHeader = FString(BEARER_PREFIX) + BlasterGameInstance->GetAccessToken();
        Request->SetHeader(AUTH_HEADER, AuthHeader);
        UE_LOG(LogTemp, Log, TEXT("SendMatchStats: Added auth header"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SendMatchStats: Failed to get GameInstance for auth token"));
    }

    Request->SetContentAsString(JsonString);

    // 응답 처리를 위한 콜백
    Request->OnProcessRequestComplete().BindLambda(
        [OnComplete](FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
        {
            if (Success && Response.IsValid())
            {
                int32 ResponseCode = Response->GetResponseCode();
                FString ResponseBody = Response->GetContentAsString();

                UE_LOG(LogTemp, Log, TEXT("SendMatchStats Response - Code: %d, Body: %s"),
                    ResponseCode, *ResponseBody);

                OnComplete.ExecuteIfBound(true, ResponseBody);
            }
            else
            {
                FString ErrorMsg = TEXT("Request failed");
                if (Response.IsValid())
                {
                    ErrorMsg = FString::Printf(TEXT("Request failed with code %d"),
                        Response->GetResponseCode());
                }

                UE_LOG(LogTemp, Error, TEXT("SendMatchStats failed: %s"), *ErrorMsg);
                OnComplete.ExecuteIfBound(false, ErrorMsg);
            }
        });

    UE_LOG(LogTemp, Log, TEXT("SendMatchStats: Sending request to %s"), *(BaseUrl + MATCH_STATS_ENDPOINT));
    Request->ProcessRequest();
}

void UBlasterWebSubsystem::OnLoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    if (bSuccess && Response.IsValid())
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            // 에러 메시지가 있는지 먼저 확인
            FString errorMessage;
            for (const auto& Field : JsonObject->Values)
            {
                if (Field.Key.Contains("Exception"))
                {
                    // 에러 메시지를 찾았으면 로그인 실패 처리
                    errorMessage = Field.Value->AsString();
                    HandleLoginFailed(errorMessage);
                    return;
                }
            }

            // 필수 필드들이 있는지 확인
            if (!JsonObject->HasField(FIELD_NICKNAME) ||
                !JsonObject->HasField(FIELD_ID) ||
                !JsonObject->HasField(FIELD_ACCESS_TOKEN) ||
                !JsonObject->HasField(FIELD_REFRESH_TOKEN))
            {
                HandleLoginFailed(TEXT("Invalid login response data"));
                return;
            }

            // 정상적인 로그인 성공 처리
            if (UGameInstance* GameInstance = GetGameInstance())
            {
                if (UBlasterGameInstance* BlasterGameInstance = Cast<UBlasterGameInstance>(GameInstance))
                {
                    BlasterGameInstance->SetNickname(JsonObject->GetStringField(FIELD_NICKNAME));
                    BlasterGameInstance->SetUserId(JsonObject->GetIntegerField(FIELD_ID));
                    BlasterGameInstance->SetAccessToken(JsonObject->GetStringField(FIELD_ACCESS_TOKEN));
                    BlasterGameInstance->SetRefreshToken(JsonObject->GetStringField(FIELD_REFRESH_TOKEN));
                }
            }
            HandleLoginSuccess();
        }
        else
        {
            HandleLoginFailed(TEXT("Failed to parse server response"));
        }
    }
    else
    {
        HandleLoginFailed(TEXT("Server connection failed"));
    }
}

void UBlasterWebSubsystem::HandleLoginSuccess()
{
    OnLoginSuccess.Broadcast();
}

void UBlasterWebSubsystem::HandleLoginFailed(const FString& ErrorMessage)
{
    UE_LOG(LogTemp, Warning, TEXT("[WebSubsystem] Login failed: %s"), *ErrorMessage);
    OnLoginFailed.Broadcast(ErrorMessage);
}
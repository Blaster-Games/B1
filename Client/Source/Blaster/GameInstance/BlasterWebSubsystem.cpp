#include "BlasterWebSubsystem.h"
#include "BlasterGameInstance.h"
#include "Json.h"
#include "JsonUtilities.h"

const TCHAR* const UBlasterWebSubsystem::LOGIN_ENDPOINT = TEXT("/api/member/login");
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

void UBlasterWebSubsystem::OnLoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    if (bSuccess && Response.IsValid())
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            // 응답 데이터 저장
            if (UGameInstance* GameInstance = GetGameInstance())
            {
                if (UBlasterGameInstance* BlasterGameInstance = Cast<UBlasterGameInstance>(GameInstance))
                {
                    BlasterGameInstance->Nickname = JsonObject->GetStringField(FIELD_NICKNAME);
                    BlasterGameInstance->UserId = JsonObject->GetIntegerField(FIELD_ID);
                    BlasterGameInstance->AccessToken = JsonObject->GetStringField(FIELD_ACCESS_TOKEN);
                    BlasterGameInstance->RefreshToken = JsonObject->GetStringField(FIELD_REFRESH_TOKEN);
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
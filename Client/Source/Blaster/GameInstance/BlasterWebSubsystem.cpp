#include "BlasterWebSubsystem.h"
#include "Json.h"
#include "JsonUtilities.h"

void UBlasterWebSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    HttpModule = &FHttpModule::Get();

    UE_LOG(LogTemp, Log, TEXT("[WebSubsystem] Initialized"));
}

void UBlasterWebSubsystem::Deinitialize()
{
    ClearAuthData();
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
            Nickname = JsonObject->GetStringField(FIELD_NICKNAME);
            UserId = JsonObject->GetIntegerField(FIELD_ID);
            AccessToken = JsonObject->GetStringField(FIELD_ACCESS_TOKEN);
            RefreshToken = JsonObject->GetStringField(FIELD_REFRESH_TOKEN);

            OnLoginSuccess();
        }
        else
        {
            OnLoginFailed(TEXT("Failed to parse server response"));
        }
    }
    else
    {
        OnLoginFailed(TEXT("Server connection failed"));
    }
}

void UBlasterWebSubsystem::OnLoginSuccess()
{
    UE_LOG(LogTemp, Log, TEXT("[WebSubsystem] Login successful for user: %s"), *Nickname);
    OnLoginSuccessDelegate.Broadcast();
}

void UBlasterWebSubsystem::OnLoginFailed(const FString& ErrorMessage)
{
    UE_LOG(LogTemp, Warning, TEXT("[WebSubsystem] Login failed: %s"), *ErrorMessage);
    ClearAuthData();
    OnLoginFailedDelegate.Broadcast(ErrorMessage);
}

void UBlasterWebSubsystem::ClearAuthData()
{
    AccessToken.Empty();
    RefreshToken.Empty();
    Nickname.Empty();
    UserId = -1;

    UE_LOG(LogTemp, Log, TEXT("[WebSubsystem] Auth data cleared"));
}
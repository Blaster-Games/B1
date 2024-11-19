#pragma once
#include "CoreMinimal.h"
#include "Blaster.h"
#include "ClientPacketHandler.h"

class BLASTER_API PacketSession : public TSharedFromThis<PacketSession>
{
public:
    PacketSession(class FSocket* Socket);
    ~PacketSession();

    UGameInstance* GetGameInstance() const
    {
        if (GEngine)
        {
            if (UWorld* World = GEngine->GetWorld())
            {
                return World->GetGameInstance();
            }
        }
        return nullptr;
    }

    void Run();
    void Stop();  // 새로 추가된 Stop 함수

    UFUNCTION(BlueprintCallable)
    void HandleRecvPackets();

    void SendPacket(SendBufferRef SendBuffer);
    void Disconnect();

public:
    class FSocket* Socket;
    TSharedPtr<class RecvWorker> RecvWorkerThread;
    TSharedPtr<class SendWorker> SendWorkerThread;
    TQueue<TArray<uint8>> RecvPacketQueue;
    TQueue<SendBufferRef> SendPacketQueue;

private:
    bool bStopThread;

public:

    bool IsRunning() const { return !bStopThread; }
};
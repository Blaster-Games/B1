
#include "Network/NetworkWorker.h"
#include "Sockets.h"
#include "Serialization/ArrayWriter.h"
#include "PacketSession.h"

// RecvWorker

RecvWorker::RecvWorker(FSocket* Socket, TSharedPtr<class PacketSession> Session) : Socket(Socket), SessionRef(Session)
{
	Thread = FRunnableThread::Create(this, TEXT("RecvWorkerThread"));
}

RecvWorker::~RecvWorker()
{

}

bool RecvWorker::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Thread Init")));
	return true;
}

uint32 RecvWorker::Run()
{
	while (Running)
	{
		TArray<uint8> Packet;
		if (ReceivePacket(OUT Packet))
		{
			if (TSharedPtr<PacketSession> Session = SessionRef.Pin())
			{
				// Enqueue 전 패킷 정보 출력
				if (Packet.Num() >= sizeof(FPacketHeader))
				{
					const FPacketHeader* Header = reinterpret_cast<const FPacketHeader*>(Packet.GetData());
					UE_LOG(LogTemp, Log, TEXT("[RecvWorker] Enqueueing Packet - ID: %d, Size: %d, Array Size: %d"),
						Header->PacketID, Header->PacketSize, Packet.Num());
				}

				Session->RecvPacketQueue.Enqueue(Packet);

				UE_LOG(LogTemp, Log, TEXT("[RecvWorker] Packet Enqueued Successfully"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[RecvWorker] Session is invalid"));
			}
		}
	}
	return 0;
}

void RecvWorker::Exit()
{

}

void RecvWorker::Destroy()
{
	Running = false;
}

bool RecvWorker::ReceivePacket(TArray<uint8>& OutPacket)
{
	// 패킷 헤더 파싱
	const int32 HeaderSize = sizeof(FPacketHeader);
	TArray<uint8> HeaderBuffer;
	HeaderBuffer.AddZeroed(HeaderSize);
	if (ReceiveDesiredBytes(HeaderBuffer.GetData(), HeaderSize) == false)
		return false;

	// ID, Size 추출
	FPacketHeader Header;
	{
		FMemoryReader Reader(HeaderBuffer);
		Reader << Header;
		UE_LOG(LogTemp, Log, TEXT("Recv PacketID : %d, PacketSize : %d"), Header.PacketID, Header.PacketSize);

		// 디버깅을 위한 추가 로그
		const int32 PayloadSize = Header.PacketSize - HeaderSize;
		UE_LOG(LogTemp, Log, TEXT("HeaderSize : %d, PayloadSize : %d"), HeaderSize, PayloadSize);
	}

	// 패킷 헤더 복사
	OutPacket = HeaderBuffer;

	// 패킷 내용 파싱
	const int32 PayloadSize = Header.PacketSize - HeaderSize;

	UE_LOG(LogTemp, Log, TEXT("Checking PayloadSize : %d"), PayloadSize);

	OutPacket.AddZeroed(PayloadSize);
	if (PayloadSize == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("PayloadSize is 0, returning true"));
		return true;
	}

	if (ReceiveDesiredBytes(&OutPacket[HeaderSize], PayloadSize))
		return true;

	UE_LOG(LogTemp, Log, TEXT("Reached end of function, returning false"));
	return false;
}

bool RecvWorker::ReceiveDesiredBytes(uint8* Results, int32 Size)
{
	if (!Socket || !Results || Size <= 0 ||
		Socket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
	{
		return false;
	}

	uint32 PendingDataSize;
	if (Socket->HasPendingData(PendingDataSize) == false || PendingDataSize <= 0)
		return false;

	int32 Offset = 0;

	while (Size > 0)
	{
		int32 NumRead = 0;
		Socket->Recv(Results + Offset, Size, OUT NumRead);
		check(NumRead <= Size);

		if (NumRead <= 0)
			return false;

		Offset += NumRead;
		Size -= NumRead;
	}

	return true;
}


// SendWorker

SendWorker::SendWorker(FSocket* Socket, TSharedPtr<PacketSession> Session) : Socket(Socket), SessionRef(Session)
{
	Thread = FRunnableThread::Create(this, TEXT("SendWorkerThread"));
}

SendWorker::~SendWorker()
{

}

bool SendWorker::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Send Thread Init")));

	return true;
}

uint32 SendWorker::Run()
{
	while (Running)
	{
		SendBufferRef SendBuffer;

		if (TSharedPtr<PacketSession> Session = SessionRef.Pin())
		{
			if (Session->SendPacketQueue.Dequeue(OUT SendBuffer))
			{
				SendPacket(SendBuffer);
			}
		}

		// Sleep?
	}

	return 0;
}

void SendWorker::Exit()
{

}

bool SendWorker::SendPacket(SendBufferRef SendBuffer)
{
	if (SendDesiredBytes(SendBuffer->Buffer(), SendBuffer->WriteSize()) == false)
		return false;

	return true;
}

void SendWorker::Destroy()
{
	Running = false;
}

bool SendWorker::SendDesiredBytes(const uint8* Buffer, int32 Size)
{
	while (Size > 0)
	{
		int32 BytesSent = 0;
		if (Socket->Send(Buffer, Size, BytesSent) == false)
			return false;

		Size -= BytesSent;
		Buffer += BytesSent;
	}

	return true;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/PacketSession.h"
#include "NetworkWorker.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "ClientPacketHandler.h"

PacketSession::PacketSession(class FSocket* Socket) : Socket(Socket)
{
	ClientPacketHandler::Init();
}

PacketSession::~PacketSession()
{
	Disconnect();
}

void PacketSession::Run()
{
	RecvWorkerThread = MakeShared<RecvWorker>(Socket, AsShared());
	SendWorkerThread = MakeShared<SendWorker>(Socket, AsShared());
}

void PacketSession::HandleRecvPackets()
{
    while (true)
    {
        TArray<uint8> Packet;
        if (RecvPacketQueue.Dequeue(OUT Packet) == false)
        {
            break;
        }

        if (Packet.Num() >= sizeof(FPacketHeader))
        {
            const FPacketHeader* Header = reinterpret_cast<const FPacketHeader*>(Packet.GetData());
            UE_LOG(LogTemp, Log, TEXT("[PacketSession] Dequeued Packet - ID: %d, Size: %d, Array Size: %d"),
                Header->PacketID, Header->PacketSize, Packet.Num());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[PacketSession] Dequeued invalid packet size: %d"), Packet.Num());
            continue;
        }

        PacketSessionRef ThisPtr = AsShared();
        UE_LOG(LogTemp, Log, TEXT("[PacketSession] Attempting to handle packet"));
        bool HandleResult = ClientPacketHandler::HandlePacket(ThisPtr, Packet.GetData(), Packet.Num());

        if (HandleResult)
        {
            UE_LOG(LogTemp, Log, TEXT("[PacketSession] Packet handled successfully"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[PacketSession] Failed to handle packet"));
        }
    }
}


void PacketSession::SendPacket(SendBufferRef SendBuffer)
{
	SendPacketQueue.Enqueue(SendBuffer);
}

void PacketSession::Disconnect()
{
	if (RecvWorkerThread)
	{
		RecvWorkerThread->Destroy();
		RecvWorkerThread = nullptr;
	}

	if (SendWorkerThread)
	{
		SendWorkerThread->Destroy();
		SendWorkerThread = nullptr;
	}
}


#include "client.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../clientside/network/datagramSocket.h"
#include "../../clientside/network/datagramPacket.h"

//client side client constructor, one on each client machine, i.e. one instance per machine.
Client::Client(const char *localIP, const char *remoteIP, int serverPort)
{
	mShape = NULL; //to be filled when we actually create the shape

	mMessage = new Message();

	mConnectionState	= mMessageDisconnected;

	mOutgoingSequence		= 1;
	mIncomingSequence		= 0;
	mIncomingAcknowledged	= 0;
	mDroppedPackets			= 0;

	// Save server's address information for later use
	mServerIP = remoteIP;
	mServerPort = serverPort;

	LogString("Server's information: IP address: %s, port: %d", remoteIP, mServerPort);

	// Create client socket
	mDatagramSocket = new DatagramSocket();
}

Client::~Client()
{
	mDatagramSocket->dreamSock_CloseSocket();
	delete mDatagramSocket;
}

void Client::Reset(void)
{
	mConnectionState = mMessageDisconnected;

    mOutgoingSequence                = 1;
    mIncomingSequence                = 0;
    mIncomingAcknowledged			 = 0;
    mDroppedPackets                  = 0;
}


void Client::DumpBuffer(void)
{
	char data[1400];
	int ret;

	while((ret = mDatagramSocket->dreamSock_GetPacket(data)) > 0)
	{
	}
}

void Client::SendConnect(const char *name)
{
	// Dump buffer so there won't be any old packets to process
	DumpBuffer();

	mConnectionState = mMessageConnecting;

	Message* message = new Message(mTempDataBuffer, sizeof(mMessage->outgoingData));
	message->WriteByte(mMessageConnect);
	message->WriteString(name);

	SendPacket(message);
}

void Client::SendDisconnect(void)
{
	Message* message = new Message(mTempDataBuffer, sizeof(mMessage->outgoingData));
	message->WriteByte(mMessageDisconnect);

	SendPacket(message);
	Reset();

	mConnectionState = mMessageDisconnecting;
}

void Client::ParsePacket(Message *mes)
{
	mes->BeginReading();
	int type = mes->ReadByte();

	// Check if the type is a positive number
	// = is the packet sequenced
	if(type > 0)
	{
		unsigned short sequence		= mes->ReadShort();
		unsigned short sequenceAck	= mes->ReadShort();

		if(sequence <= mIncomingSequence)
		{
			LogString("Client: (sequence: %d <= incoming seq: %d)",
				sequence, mIncomingSequence);

			LogString("Client: Sequence mismatch");
		}

		mDroppedPackets = sequence - mIncomingSequence + 1;

		mIncomingSequence = sequence;
		mIncomingAcknowledged = sequenceAck;
	}

	// Parse trough the system messages
	switch(type)
	{
	case mMessageConnect:
		mConnectionState = mMessageConnected;

		LogString("LIBRARY: Client: got connect confirmation");
		break;

	case mMessageDisconnect:
		mConnectionState = mMessageDisconnected;

		LogString("LIBRARY: Client: got disconnect confirmation");
		break;

	case mMessageAddShape:
		//LogString("LIBRARY: Client: adding a shape");
		break;

	case mMessageRemoveShape:
		LogString("LIBRARY: Client: removing a client");
		break;
	}
}

int Client::GetPacket(char *data)
{
	// Check if the client is set up or if it is disconnecting
	if(!mDatagramSocket->mSocket)
		return 0;

	int ret;

	Message* message = new Message(mMessage->outgoingData, sizeof(mMessage->outgoingData));

	//mes.Init(data, sizeof(data));

	ret = mDatagramSocket->dreamSock_GetPacket(message->data);

	if(ret <= 0)
		return 0;

	message->SetSize(ret);

	// Parse system messages
	ParsePacket(message);

	return ret;
}

void Client::SendPacket(Message *theMes)
{
	// Check that everything is set up
	if(!mDatagramSocket->mSocket || mConnectionState == mMessageDisconnected)
	{
		LogString("SendPacket error: Could not send because the client is disconnected");
		return;
	}

	// If the message overflowed do not send it
	if(theMes->GetOverFlow())
	{
		LogString("SendPacket error: Could not send because the buffer overflowed");
		return;
	}

	  DatagramPacket* packet = new DatagramPacket(theMes,theMes->data,sizeof(theMes->data),mServerIP,mServerPort);
	//DatagramPacket* packet = new DatagramPacket(       theMes->data,sizeof(theMes->data),mServerIP,mServerPort);
	
	mDatagramSocket->send(packet);

	theMes->BeginReading();
	int type = theMes->ReadByte();

	if(type > 0)
	{
		mOutgoingSequence++;
	}
}



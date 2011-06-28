#include "client.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../network/network.h"
#ifdef WIN32
//
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

//server side client constructor, many instances will be made, one for each client connected.
Client::Client(Network* network)
{
	mNetwork = network;

	mShape = NULL; //to be filled when we actually create the shape

	connectionState	= DREAMSOCK_DISCONNECTED;

	outgoingSequence		= 1;
	incomingSequence		= 0;
	incomingAcknowledged	= 0;
	mDroppedPackets			= 0;

	lastMessageTime			= 0;
}

Client::~Client()
{
	mNetwork->dreamSock_CloseSocket(mNetwork->mSocket);
	delete mNetwork;
}

void Client::DumpBuffer(void)
{
	char data[1400];
	int ret;

	while((ret = mNetwork->dreamSock_GetPacket(mNetwork->mSocket, data, NULL)) > 0)
	{
	}
}

void Client::SendPacket(void)
{
	// Check that everything is set up
	if(!mNetwork->mSocket || connectionState == DREAMSOCK_DISCONNECTED)
	{
		LogString("SendPacket error: Could not send because the client is disconnected");
		return;
	}

	// If the message overflowed, do not send it
	if(mMessage.GetOverFlow())
	{
		LogString("SendPacket error: Could not send because the buffer overflowed");
		return;
	}

	mNetwork->dreamSock_SendPacket(mNetwork->mSocket, mMessage.GetSize(), mMessage.data, myaddress);

	// Check if the packet is sequenced
	mMessage.BeginReading();
	int type = mMessage.ReadByte();

	if(type > 0)
	{
		outgoingSequence++;
	}
}

void Client::SendPacket(Message *theMes)
{
	// Check that everything is set up
	if(!mNetwork->mSocket || connectionState == DREAMSOCK_DISCONNECTED)
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

	mNetwork->dreamSock_SendPacket(mNetwork->mSocket, theMes->GetSize(), theMes->data, myaddress);

	// Check if the packet is sequenced
	theMes->BeginReading();
	int type = theMes->ReadByte();

	if(type > 0)
	{
		outgoingSequence++;
	}
}



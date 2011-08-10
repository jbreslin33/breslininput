#include "client.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../../serverside/network/network.h"
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
	mNetwork = network; //handles udp sockets

	mShape = NULL; //to be filled when we actually create the shape

	mConnectionState	= DREAMSOCK_DISCONNECTED;

	mOutgoingSequence		= 1;
	mIncomingSequence		= 0;
	mDroppedPackets			= 0;

	mLastMessageTime		= 0;
}

Client::~Client()
{
	mNetwork->dreamSock_CloseSocket(mNetwork->mSocket);
	delete mNetwork;
}

void Client::SendPacket(Message *theMes)
{
	// Check that everything is set up
	if(!mNetwork->mSocket || mConnectionState == DREAMSOCK_DISCONNECTED)
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

	mNetwork->dreamSock_SendPacket(mNetwork->mSocket, theMes->GetSize(), theMes->data, mMyaddress);

	// Check if the packet is sequenced
	theMes->BeginReading();
	int type = theMes->ReadByte();

	if(type > 0)
	{
		mOutgoingSequence++;
	}
}



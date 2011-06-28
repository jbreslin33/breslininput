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

//client side client constructor, one on each client machine, i.e. one instance per machine.
Client::Client(const char *localIP, const char *remoteIP, int port)
{
	mShape = NULL; //to be filled when we actually create the shape

	mConnectionState	= DREAMSOCK_DISCONNECTED;

	mOutgoingSequence		= 1;
	mIncomingSequence		= 0;
	mIncomingAcknowledged	= 0;
	mDroppedPackets			= 0;
	mLastMessageTime			= 0;

	// Save server's address information for later use
	mServerPort = port;
	
	#ifdef WIN32
	size_t t = 256;
	strcpy_s(mServerIP,t, remoteIP);
#else
	strcpy(serverIP,remoteIP);
#endif
	LogString("Server's information: IP address: %s, port: %d", mServerIP, mServerPort);

	// Create client socket
	mNetwork = new Network(this,localIP, 0);

	// Check that the address is not empty
	u_long inetAddr = inet_addr(mServerIP);

	if(inetAddr == INADDR_NONE)
	{
		LogString("DREAMSOCK_CLIENT_ERROR");
	}

	if(mNetwork->mSocket == DREAMSOCK_INVALID_SOCKET)
	{
		LogString("DREAMSOCK_CLIENT_ERROR");
	}
}

Client::~Client()
{
	mNetwork->dreamSock_CloseSocket(mNetwork->mSocket);
	delete mNetwork;
}

void Client::Reset(void)
{
	mConnectionState = DREAMSOCK_DISCONNECTED;

    mOutgoingSequence                = 1;
    mIncomingSequence                = 0;
    mIncomingAcknowledged    = 0;
    mDroppedPackets                  = 0;
    mLastMessageTime                 = 0;
}


void Client::DumpBuffer(void)
{
	char data[1400];
	int ret;

	while((ret = mNetwork->dreamSock_GetPacket(mNetwork->mSocket, data, NULL)) > 0)
	{
	}
}

void Client::SendConnect(const char *name)
{
	// Dump buffer so there won't be any old packets to process
	DumpBuffer();

	mConnectionState = DREAMSOCK_CONNECTING;

	mMessage.Init(mMessage.outgoingData, sizeof(mMessage.outgoingData));
	mMessage.WriteByte(DREAMSOCK_MES_CONNECT);
	mMessage.WriteString(name);

	SendPacket(&mMessage);
}

void Client::SendDisconnect(void)
{
	mMessage.Init(mMessage.outgoingData, sizeof(mMessage.outgoingData));
	mMessage.WriteByte(DREAMSOCK_MES_DISCONNECT);

	SendPacket(&mMessage);
	Reset();

	mConnectionState = DREAMSOCK_DISCONNECTING;
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
	case DREAMSOCK_MES_CONNECT:
		mConnectionState = DREAMSOCK_CONNECTED;

		LogString("LIBRARY: Client: got connect confirmation");
		break;

	case DREAMSOCK_MES_DISCONNECT:
		mConnectionState = DREAMSOCK_DISCONNECTED;

		LogString("LIBRARY: Client: got disconnect confirmation");
		break;

	case DREAMSOCK_MES_ADDSHAPE:
		LogString("LIBRARY: Client: adding a shape");
		break;

	case DREAMSOCK_MES_REMOVESHAPE:
		LogString("LIBRARY: Client: removing a client");
		break;
	}
}

int Client::GetPacket(char *data, struct sockaddr *from)
{
	// Check if the client is set up or if it is disconnecting
	if(!mNetwork->mSocket)
		return 0;

	int ret;

	Message mes;
	//LogString("sizeOfData:%d",sizeof(data));
	mes.Init(data, sizeof(data));

	ret = mNetwork->dreamSock_GetPacket(mNetwork->mSocket, mes.data, from);

	if(ret <= 0)
		return 0;
	//LogString("size:%d",mes.GetSize());
	mes.SetSize(ret);

	// Parse system messages
	ParsePacket(&mes);

	return ret;
}

void Client::SendPacket(void)
{
	// Check that everything is set up
	if(!mNetwork->mSocket || mConnectionState == DREAMSOCK_DISCONNECTED)
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

	// Check if serverPort is set. If it is, we are a client sending to the server.
	// Otherwise we are a server sending to a client.
	if(mServerPort)
	{
		struct sockaddr_in sendToAddress;
		memset((char *) &sendToAddress, 0, sizeof(sendToAddress));

		u_long inetAddr = inet_addr(mServerIP);
		sendToAddress.sin_port = htons((u_short) mServerPort);
		sendToAddress.sin_family = AF_INET;
		sendToAddress.sin_addr.s_addr = inetAddr;

		mNetwork->dreamSock_SendPacket(mNetwork->mSocket, mMessage.GetSize(), mMessage.data,
			*(struct sockaddr *) &sendToAddress);
	}

	// Check if the packet is sequenced
	mMessage.BeginReading();
	int type = mMessage.ReadByte();

	if(type > 0)
	{
		mOutgoingSequence++;
	}
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

	// Check if serverPort is set. If it is, we are a client sending to the server.
	// Otherwise we are a server sending to a client.
	if(mServerPort)
	{
		struct sockaddr_in sendToAddress;
		memset((char *) &sendToAddress, 0, sizeof(sendToAddress));

		u_long inetAddr = inet_addr(mServerIP);
		sendToAddress.sin_port = htons((u_short) mServerPort);
		sendToAddress.sin_family = AF_INET;
		sendToAddress.sin_addr.s_addr = inetAddr;

		mNetwork->dreamSock_SendPacket(mNetwork->mSocket, theMes->GetSize(), theMes->data,
			*(struct sockaddr *) &sendToAddress);
	}

	// Check if the packet is sequenced
	theMes->BeginReading();
	int type = theMes->ReadByte();

	if(type > 0)
	{
		mOutgoingSequence++;
	}
}



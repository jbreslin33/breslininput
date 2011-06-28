#include "client.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../network/network.h"
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
	droppedPackets			= 0;

	serverPort				= 0;

	pingSent				= 0;
	ping					= 0;

	lastMessageTime			= 0;
}

//client side client constructor, one on each client machine, i.e. one instance per machine.
Client::Client(const char *localIP, const char *remoteIP, int port)
{
	mShape = NULL; //to be filled when we actually create the shape

	connectionState	= DREAMSOCK_DISCONNECTED;

	outgoingSequence		= 1;
	incomingSequence		= 0;
	incomingAcknowledged	= 0;
	droppedPackets			= 0;

	serverPort				= 0;

	pingSent				= 0;
	ping					= 0;

	lastMessageTime			= 0;

	// Save server's address information for later use
	serverPort = port;
	
	#ifdef WIN32
	size_t t = 256;
	strcpy_s(serverIP,t, remoteIP);
#else
	strcpy(serverIP,remoteIP);
#endif
	LogString("Server's information: IP address: %s, port: %d", serverIP, serverPort);

	// Create client socket
	mNetwork = new Network(this,localIP, 0);

	// Check that the address is not empty
	u_long inetAddr = inet_addr(serverIP);

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

void Client::Uninitialise(void)
{
	mNetwork->dreamSock_CloseSocket(mNetwork->mSocket);
	Reset();
}

void Client::Reset(void)
{
	connectionState = DREAMSOCK_DISCONNECTED;

	outgoingSequence		= 1;
	incomingSequence		= 0;
	incomingAcknowledged	= 0;
	droppedPackets			= 0;

	pingSent				= 0;
	ping					= 0;

	lastMessageTime			= 0;

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

	connectionState = DREAMSOCK_CONNECTING;

	mMessage.Init(mMessage.outgoingData, sizeof(mMessage.outgoingData));
	mMessage.WriteByte(DREAMSOCK_MES_CONNECT);
	mMessage.WriteString(name);

	SendPacket(&mMessage);
}

void Client::forceUpdate()
{

}

void Client::SendDisconnect(void)
{
	mMessage.Init(mMessage.outgoingData, sizeof(mMessage.outgoingData));
	mMessage.WriteByte(DREAMSOCK_MES_DISCONNECT);

	SendPacket(&mMessage);
	Reset();

	connectionState = DREAMSOCK_DISCONNECTING;
}

void Client::SendPing(void)
{
	pingSent = mNetwork->dreamSock_GetCurrentSystemTime();

	mMessage.Init(mMessage.outgoingData, sizeof(mMessage.outgoingData));
	mMessage.WriteByte(DREAMSOCK_MES_PING);

	SendPacket(&mMessage);
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

		if(sequence <= incomingSequence)
		{
			LogString("Client: (sequence: %d <= incoming seq: %d)",
				sequence, incomingSequence);

			LogString("Client: Sequence mismatch");
		}

		droppedPackets = sequence - incomingSequence + 1;

		incomingSequence = sequence;
		incomingAcknowledged = sequenceAck;
	}

	// Parse trough the system messages
	switch(type)
	{
	case DREAMSOCK_MES_CONNECT:
		connectionState = DREAMSOCK_CONNECTED;

		LogString("LIBRARY: Client: got connect confirmation");
		break;

	case DREAMSOCK_MES_DISCONNECT:
		connectionState = DREAMSOCK_DISCONNECTED;

		LogString("LIBRARY: Client: got disconnect confirmation");
		break;

	case DREAMSOCK_MES_ADDSHAPE:
		LogString("LIBRARY: Client: adding a shape");
		break;

	case DREAMSOCK_MES_REMOVESHAPE:
		LogString("LIBRARY: Client: removing a client");
		break;

	case DREAMSOCK_MES_PING:
		SendPing();
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

	// Check if serverPort is set. If it is, we are a client sending to the server.
	// Otherwise we are a server sending to a client.
	if(serverPort)
	{
		struct sockaddr_in sendToAddress;
		memset((char *) &sendToAddress, 0, sizeof(sendToAddress));

		u_long inetAddr = inet_addr(serverIP);
		sendToAddress.sin_port = htons((u_short) serverPort);
		sendToAddress.sin_family = AF_INET;
		sendToAddress.sin_addr.s_addr = inetAddr;

		mNetwork->dreamSock_SendPacket(mNetwork->mSocket, mMessage.GetSize(), mMessage.data,
			*(struct sockaddr *) &sendToAddress);
	}
	else
	{
		mNetwork->dreamSock_SendPacket(mNetwork->mSocket, mMessage.GetSize(), mMessage.data, myaddress);
	}

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

	// Check if serverPort is set. If it is, we are a client sending to the server.
	// Otherwise we are a server sending to a client.
	if(serverPort)
	{
		struct sockaddr_in sendToAddress;
		memset((char *) &sendToAddress, 0, sizeof(sendToAddress));

		u_long inetAddr = inet_addr(serverIP);
		sendToAddress.sin_port = htons((u_short) serverPort);
		sendToAddress.sin_family = AF_INET;
		sendToAddress.sin_addr.s_addr = inetAddr;

		mNetwork->dreamSock_SendPacket(mNetwork->mSocket, theMes->GetSize(), theMes->data,
			*(struct sockaddr *) &sendToAddress);
	}
	else
	{
		mNetwork->dreamSock_SendPacket(mNetwork->mSocket, theMes->GetSize(), theMes->data, myaddress);
	}

	// Check if the packet is sequenced
	theMes->BeginReading();
	int type = theMes->ReadByte();

	if(type > 0)
	{
		outgoingSequence++;
	}
}



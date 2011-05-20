//
// DreamSock network-library
//
// Programmers:
// Teijo Hakala
//
/////////////////////////////////////////////////////////

//#define WIN32

#include "../message/message.h"

#ifdef WIN32
// Windows specific headers
	#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_
	#include <windows.h>
	#include <winsock2.h>

	#endif
#else
// UNIX specific headers
	#include <memory.h>
	#include <errno.h>
	#include <sys/ioctl.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif

// Common headers
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "dreamSock.h"
#include "dreamSockLog.h"



bool dreamSock_init = false;

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
int dreamSock_Initialize(void)
{
	if(dreamSock_init == true)
		return 0;

	dreamSock_init = true;

	StartLog();

#ifdef WIN32
	return dreamSock_InitializeWinSock();
#else
	return 0;
#endif
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void dreamSock_Shutdown(void)
{
	if(dreamSock_init == false)
		return;

	LogString("Shutting down dreamSock");

	dreamSock_init = false;

	StopLog();

#ifdef WIN32
	WSACleanup();
#endif
}

dreamClient::dreamClient()
{

	mServerSidePlayer = NULL;
	mClientSidePlayer = NULL;

	connectionState	= DREAMSOCK_DISCONNECTED;

	outgoingSequence		= 1;
	incomingSequence		= 0;
	incomingAcknowledged	= 0;
	droppedPackets			= 0;

	init					= false;

	serverPort				= 0;

	pingSent				= 0;
	ping					= 0;

	lastMessageTime			= 0;
}

dreamClient::~dreamClient()
{
	dreamSock_CloseSocket(socket);
}

int dreamClient::Initialise(const char *localIP, const char *remoteIP, int port)
{
//	LogString("initilisinfdf");
	// Initialise dreamSock if it is not already initialised
	dreamSock_Initialize();

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
	socket = dreamSock_OpenUDPSocket(localIP, 0);

	// Check that the address is not empty
	u_long inetAddr = inet_addr(serverIP);

	if(inetAddr == INADDR_NONE)
	{
		return DREAMSOCK_CLIENT_ERROR;
	}

	if(socket == DREAMSOCK_INVALID_SOCKET)
	{
		return DREAMSOCK_CLIENT_ERROR;
	}

	init = true;

	return 0;
}

void dreamClient::Uninitialise(void)
{
	dreamSock_CloseSocket(socket);

	Reset();

	init = false;
}

void dreamClient::Reset(void)
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

void dreamClient::DumpBuffer(void)
{
	char data[1400];
	int ret;

	while((ret = dreamSock_GetPacket(socket, data, NULL)) > 0)
	{
	}
}

void dreamClient::SendConnect(const char *name)
{
	LogString("sending connect...");
	// Dump buffer so there won't be any old packets to process
	DumpBuffer();

	connectionState = DREAMSOCK_CONNECTING;

	mMessage.Init(mMessage.outgoingData, sizeof(mMessage.outgoingData));
	mMessage.WriteByte(DREAMSOCK_MES_CONNECT);
	mMessage.WriteString(name);

	SendPacket(&mMessage);
}

void dreamClient::SendDisconnect(void)
{
	mMessage.Init(mMessage.outgoingData, sizeof(mMessage.outgoingData));
	mMessage.WriteByte(DREAMSOCK_MES_DISCONNECT);

	SendPacket(&mMessage);
	Reset();

	connectionState = DREAMSOCK_DISCONNECTING;
}

void dreamClient::SendPing(void)
{
	pingSent = dreamSock_GetCurrentSystemTime();

	mMessage.Init(mMessage.outgoingData, sizeof(mMessage.outgoingData));
	mMessage.WriteByte(DREAMSOCK_MES_PING);

	SendPacket(&mMessage);
}

void dreamClient::ParsePacket(Message *mes)
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

	case DREAMSOCK_MES_ADDCLIENT:
		LogString("LIBRARY: Client: adding a client");
		break;

	case DREAMSOCK_MES_REMOVECLIENT:
		LogString("LIBRARY: Client: removing a client");
		break;

	case DREAMSOCK_MES_PING:
		SendPing();
		break;
	}
}

int dreamClient::GetPacket(char *data, struct sockaddr *from)
{
	//LogString("getPtcadd");
	// Check if the client is set up or if it is disconnecting
	if(!socket)
		return 0;

	int ret;

	Message mes;
	mes.Init(data, sizeof(data));

	ret = dreamSock_GetPacket(socket, mes.data, from);

	if(ret <= 0)
		return 0;

	mes.SetSize(ret);

	// Parse system messages
	ParsePacket(&mes);

	//LogString("still good");
	return ret;
}

void dreamClient::SendPacket(void)
{
	// Check that everything is set up
	if(!socket || connectionState == DREAMSOCK_DISCONNECTED)
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

		dreamSock_SendPacket(socket, mMessage.GetSize(), mMessage.data,
			*(struct sockaddr *) &sendToAddress);
	}
	else
	{
		dreamSock_SendPacket(socket, mMessage.GetSize(), mMessage.data, myaddress);
	}

	// Check if the packet is sequenced
	mMessage.BeginReading();
	int type = mMessage.ReadByte();

	if(type > 0)
	{
		outgoingSequence++;
	}
}

void dreamClient::SendPacket(Message *theMes)
{
	// Check that everything is set up
	if(!socket || connectionState == DREAMSOCK_DISCONNECTED)
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

		dreamSock_SendPacket(socket, theMes->GetSize(), theMes->data,
			*(struct sockaddr *) &sendToAddress);
	}
	else
	{
		dreamSock_SendPacket(socket, theMes->GetSize(), theMes->data, myaddress);
	}

	// Check if the packet is sequenced
	theMes->BeginReading();
	int type = theMes->ReadByte();

	if(type > 0)
	{
		outgoingSequence++;
	}
}


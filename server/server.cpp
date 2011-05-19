//
// DreamSock network-library
//
// Programmers:
// Teijo Hakala
//
/////////////////////////////////////////////////////////

//#define WIN32
#include "server.h"
#include "../tdreamsock/dreamSock.h"
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
#include "../tdreamsock/dreamSock.h"
#include "../tdreamsock/dreamSockLog.h"

Server::Server()
{
	init			= false;

	port			= 0;
	runningIndex	= 1;
	socket			= 0;
}

Server::~Server()
{
	mClientVector.empty();
	dreamSock_CloseSocket(socket);
}

int Server::Initialise(const char *localIP, int serverPort)
{
	// Initialise dreamSock if it is not already initialised
	dreamSock_Initialize();

	// Store the server IP and port for later use
	port = serverPort;

	// Create server socket
	socket = dreamSock_OpenUDPSocket(localIP, port);

	if(socket == DREAMSOCK_INVALID_SOCKET)
	{
		return DREAMSOCK_SERVER_ERROR;
	}

	init = true;

	return 0;
}

void Server::Uninitialise(void)
{
	dreamSock_CloseSocket(socket);

	init = false;
}

void Server::SendAddClient(dreamClient *newClient)
{
	// Send connection confirmation
	newClient->mMessage.Init(newClient->mMessage.outgoingData,
		sizeof(newClient->mMessage.outgoingData));

	newClient->mMessage.WriteByte(DREAMSOCK_MES_CONNECT);	// type
	newClient->SendPacket();

	// Send 'Add client' message to every client

	// First inform the new client of the other clients
	for (unsigned int i = 0; i < mClientVector.size(); i++)
	{
		LogString("inform new client of others");
		newClient->mMessage.Init(newClient->mMessage.outgoingData,
			sizeof(newClient->mMessage.outgoingData));

		newClient->mMessage.WriteByte(DREAMSOCK_MES_ADDCLIENT); // type

		if(mClientVector.size() == 0)
		{

			LogString("LOCAL Y");
			newClient->mMessage.WriteByte(1);	// local client
			newClient->mMessage.WriteByte(mClientVector.at(i)->GetIndex());
			newClient->mMessage.WriteString(mClientVector.at(i)->GetName());
		}
		else
		{
						LogString("LOCAL N");
			newClient->mMessage.WriteByte(0);	// not-local client
			newClient->mMessage.WriteByte(mClientVector.at(i)->GetIndex());
			newClient->mMessage.WriteString(mClientVector.at(i)->GetName());
		}

		newClient->SendPacket();
	}

	// Then tell the others about the new client
	for (unsigned int i = 0; i < mClientVector.size(); i++)
	{
		if(mClientVector.at(i) == newClient)
			continue;
		LogString("telling others of new client");
		mClientVector.at(i)->mMessage.Init(mClientVector.at(i)->mMessage.outgoingData,
			sizeof(mClientVector.at(i)->mMessage.outgoingData));

		mClientVector.at(i)->mMessage.WriteByte(DREAMSOCK_MES_ADDCLIENT); // type

		mClientVector.at(i)->mMessage.WriteByte(0);
		mClientVector.at(i)->mMessage.WriteByte(newClient->GetIndex());
		mClientVector.at(i)->mMessage.WriteString(newClient->GetName());

		mClientVector.at(i)->SendPacket();
	}
}

void Server::SendRemoveClient(dreamClient *client)
{
	int index = client->GetIndex();

	// Send 'Remove client' message to every client
	for (unsigned int i = 0; i < mClientVector.size(); i++)
	{
		mClientVector.at(i)->mMessage.Init(mClientVector.at(i)->mMessage.outgoingData,
			sizeof(mClientVector.at(i)->mMessage.outgoingData));

		mClientVector.at(i)->mMessage.WriteByte(DREAMSOCK_MES_REMOVECLIENT);	// type
		mClientVector.at(i)->mMessage.WriteByte(index);							// index
	}

	SendPackets();

	// Send disconnection confirmation
	client->mMessage.Init(client->mMessage.outgoingData,
		sizeof(client->mMessage.outgoingData));

	client->mMessage.WriteByte(DREAMSOCK_MES_DISCONNECT);
	client->SendPacket();
}

void Server::SendPing(void)
{
	// Send ping message to every client
	for (unsigned int i = 0; i < mClientVector.size(); i++)
	{
		mClientVector.at(i)->SendPing();
	}
}

void Server::AddClient(struct sockaddr *address, char *name)
{
	// First get a pointer to the beginning of client list
	//dreamClient *list = clientList;
	//dreamClient *prev;
	//dreamClient *newClient;

	LogString("LIB: Adding client, index %d", runningIndex);

	// No clients yet, adding the first one
	if(mClientVector.size() == 0)
	{
		LogString("LIB: Server: Adding first client");

		clientList = (dreamClient *) calloc(1, sizeof(dreamClient));

		clientList->SetSocket(socket);
		clientList->SetSocketAddress(address);

		clientList->SetConnectionState(DREAMSOCK_CONNECTING);
		clientList->SetOutgoingSequence(1);
		clientList->SetIncomingSequence(0);
		clientList->SetIncomingAcknowledged(0);
		clientList->SetIndex(runningIndex);
		clientList->SetName(name);
		clientList->next = NULL;

		newClient = clientList;
	}
	else
	{
		LogString("LIB: Server: Adding another client");

		prev = list;
		list = clientList->next;

		while(list != NULL)
		{
			prev = list;
			list = list->next;
		}

		list = (dreamClient *) calloc(1, sizeof(dreamClient));

		list->SetSocket(socket);
		list->SetSocketAddress(address);

		list->SetConnectionState(DREAMSOCK_CONNECTING);
		list->SetOutgoingSequence(1);
		list->SetIncomingSequence(0);
		list->SetIncomingAcknowledged(0);
		list->SetIndex(runningIndex);
		list->SetName(name);
		list->next = NULL;

		prev->next = list;

		newClient = list;
	}

	runningIndex++;

	SendAddClient(newClient);
}

void Server::RemoveClient(dreamClient *client)
{
	dreamClient *list = NULL;
	dreamClient *prev = NULL;
	dreamClient *next = NULL;

	int index = client->GetIndex();

	LogString("LIB: Removing client with index %d", index);

	SendRemoveClient(client);

	for(list = clientList; list != NULL; list = list->next)
	{
		if(client == list)
		{
			if(prev != NULL)
			{
				prev->next = client->next;
			}

			break;
		}

		prev = list;
	}

	if(client == clientList)
	{
		LogString("LIB: Server: removing first client in list");

		if(list) next = list->next;

		if(client) free(client);
		client = NULL;
		clientList = next;
	}
	else
	{
		LogString("LIB: Server: removing a client");

		if(list) next = list->next;

		if(client) free(client);
		client = next;
	}
}

void Server::ParsePacket(Message *mes, struct sockaddr *address)
{
	mes->BeginReading();
	int type = mes->ReadByte();

	// Find the correct client by comparing addresses
	dreamClient *clList = clientList;

	// If we do not have clients yet, skip to message type checking
	if(clList != NULL )
	{
		for( ; clList != NULL; clList = clList->next)
		{
			if(memcmp(clList->GetSocketAddress(), address, sizeof(address)) == 0)
			{
				break;
			}
		}

		if(clList != NULL)
		{
			clList->SetLastMessageTime(dreamSock_GetCurrentSystemTime());

			// Check if the type is a positive number
			// -> is the packet sequenced
			if(type > 0)
			{
				unsigned short sequence		= mes->ReadShort();
				unsigned short sequenceAck	= mes->ReadShort();

				if(sequence <= clList->GetIncomingSequence())
				{
					LogString("LIB: Server: Sequence mismatch (sequence: %ld <= incoming seq: %ld)",
						sequence, clList->GetIncomingSequence());
				}

				clList->SetDroppedPackets(sequence - clList->GetIncomingSequence() - 1);

				clList->SetIncomingSequence(sequence);
				clList->SetIncomingAcknowledged(sequenceAck);
			}

			// Wait for one message before setting state to connected
			if(clList->GetConnectionState() == DREAMSOCK_CONNECTING)
				clList->SetConnectionState(DREAMSOCK_CONNECTED);
		}
	}

	// Parse through the system messages
	switch(type)
	{
	case DREAMSOCK_MES_CONNECT:
		AddClient(address, mes->ReadString());

		LogString("LIBRARY: Server: a client connected succesfully");
		break;

	case DREAMSOCK_MES_DISCONNECT:
		if(clList == NULL)
			break;

		RemoveClient(clList);

		LogString("LIBRARY: Server: a client disconnected");
		break;

	case DREAMSOCK_MES_PING:
		clList->SetPing(dreamSock_GetCurrentSystemTime() - clList->GetPingSent());
		break;
	}
}

int Server::CheckForTimeout(char *data, struct sockaddr *from)
{
	int currentTime = dreamSock_GetCurrentSystemTime();

	dreamClient *clList = clientList;
	dreamClient *next;

	for( ; clList != NULL;)
	{
		next = clList->next;

		// Don't timeout when connecting
		if(clList->GetConnectionState() == DREAMSOCK_CONNECTING)
		{
			clList = next;
			continue;
		}

		// Check if the client has been silent for 30 seconds
		// If yes, assume crashed and remove the client
		if(currentTime - clList->GetLastMessageTime() > 30000)
		{
			LogString("Client timeout, disconnecting (%d - %d = %d)",
				currentTime, clList->GetLastMessageTime(), currentTime - clList->GetLastMessageTime());

			// Build a 'fake' message so the application will also
			// receive notification of a client disconnecting
			Message mes;
			mes.Init(data, sizeof(data));
			mes.WriteByte(DREAMSOCK_MES_DISCONNECT);

			*(struct sockaddr *) from = *clList->GetSocketAddress();

			RemoveClient(clList);

			return mes.GetSize();
		}

		clList = next;
	}

	return 0;
}

int Server::GetPacket(char *data, struct sockaddr *from)
{
	// Check if the server is set up
	if(!socket)
		return 0;

	// Check for timeout
	int timeout = CheckForTimeout(data, from);

	if(timeout)
		return timeout;

	// Wait for a while or incoming data
	int maxfd = socket;
	fd_set allset;
	struct timeval waittime;

	waittime.tv_sec = 10 / 1000;
	waittime.tv_usec = (10 % 1000) * 1000;

	FD_ZERO(&allset); 
	FD_SET(socket, &allset);

	fd_set reading = allset;

	int nready = select(maxfd + 1, &reading, NULL, NULL, &waittime);

	if(!nready)
		return 0;

	// Read data of the socket
	int ret = 0;

	Message mes;
	mes.Init(data, sizeof(data));

	ret = dreamSock_GetPacket(socket, mes.data, from);

	if(ret <= 0)
		return 0;

	mes.SetSize(ret);

	// Parse system messages
	ParsePacket(&mes, from);

	return ret;
}

void Server::SendPackets(void)
{
	// Check if the server is set up
	if(!socket)
		return;

	dreamClient *clList = clientList;

	for( ; clList != NULL; clList = clList->next)
	{
		if(clList->mMessage.GetSize() == 0)
			continue;

		clList->SendPacket();
	}
}

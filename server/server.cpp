
#include "server.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../network/network.h"
#include "../client/client.h"
#include "../message/message.h"

#include "../game/serverSideGame.h"

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



Server::Server(ServerSideGame* serverSideGame)
{
	mNetwork = new Network();
	init			= false;
	mServerSideGame = serverSideGame;
	port			= 0;
	runningIndex	= 1;
	socket			= 0;
}

Server::~Server()
{
	mClientVector.empty();
	mNetwork->dreamSock_CloseSocket(socket);
}

int Server::Initialise(const char *localIP, int serverPort)
{
	// Initialise dreamSock if it is not already initialised
	mNetwork->dreamSock_Initialize();

	// Store the server IP and port for later use
	port = serverPort;

	// Create server socket
	socket = mNetwork->dreamSock_OpenUDPSocket(localIP, port);

	if(socket == DREAMSOCK_INVALID_SOCKET)
	{
		return DREAMSOCK_SERVER_ERROR;
	}

	init = true;

	return 0;
}

void Server::Uninitialise(void)
{
	mNetwork->dreamSock_CloseSocket(socket);

	init = false;
}

void Server::SendAddClient(Client *newClient)
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

		if(mClientVector.at(i) == newClient)
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
		{
			LogString("this is the same client, don't inform");
			continue;
		}
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

void Server::SendRemoveClient(Client *client)
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
	LogString("LIB: Adding client, index %d", runningIndex);
	Client* client = new Client();
	mClientVector.push_back(client);
	// No clients yet, adding the first one

	LogString("LIB: Server: Adding first client");

	//clientList = (dreamClient *) calloc(1, sizeof(dreamClient));

	client->SetSocket(socket);
	client->SetSocketAddress(address);
	client->SetConnectionState(DREAMSOCK_CONNECTING);
	client->SetOutgoingSequence(1);
	client->SetIncomingSequence(0);
	client->SetIncomingAcknowledged(0);
	client->SetIndex(runningIndex);
	client->SetName(name);

	memcpy(&client->myaddress,client->GetSocketAddress(), sizeof(struct sockaddr));

	mServerSideGame->createPlayer(client,runningIndex);

	runningIndex++;

	SendAddClient(client);
}

void Server::RemoveClient(Client *client)
{
	
	for (unsigned int i = 0; i < mClientVector.size(); i++)
	{
		if (mClientVector.at(i) == client)
		{
			mClientVector.erase (mClientVector.begin()+i);
		}
	}
	/*
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
	*/
}

void Server::ParsePacket(Message *mes, struct sockaddr *address)
{
        mes->BeginReading();
        int type = mes->ReadByte();

        // Find the correct client by comparing addresses
  
        // If we do not have clients yet, skip to message type checking
		
		unsigned int i = 0;
		if (mClientVector.size() > 0)
			//LogString("vector is not null");
        //if(clList != NULL )
        {
  				for (i = 0; i < mClientVector.size(); i++)
				{
						if(memcmp(mClientVector.at(i)->GetSocketAddress(), address, sizeof(address)) == 0)
                        {
                                break;
                        }
                }
				if (mClientVector.size() > 0)
               // if(clList != NULL)
                {
                        mClientVector.at(i)->SetLastMessageTime(mNetwork->dreamSock_GetCurrentSystemTime());

                        // Check if the type is a positive number
                        // -> is the packet sequenced
                        if(type > 0)
                        {
                                unsigned short sequence         = mes->ReadShort();
                                unsigned short sequenceAck      = mes->ReadShort();

                                if(sequence <= mClientVector.at(i)->GetIncomingSequence())
                                {
                                        LogString("LIB: Server: Sequence mismatch (sequence: %ld <= incoming seq: %ld)",
                                                sequence, mClientVector.at(i)->GetIncomingSequence());
                                }

                                mClientVector.at(i)->SetDroppedPackets(sequence - mClientVector.at(i)->GetIncomingSequence() - 1);

                                mClientVector.at(i)->SetIncomingSequence(sequence);
                                mClientVector.at(i)->SetIncomingAcknowledged(sequenceAck);
                        }

                        // Wait for one message before setting state to connected
                        if(mClientVector.at(i)->GetConnectionState() == DREAMSOCK_CONNECTING)
                                mClientVector.at(i)->SetConnectionState(DREAMSOCK_CONNECTED);
                }
        }
		//LogString("heere");
        // Parse through the system messages
        switch(type)
        {
        case DREAMSOCK_MES_CONNECT:
                AddClient(address, mes->ReadString());

                LogString("LIBRARY: Server: a client connected succesfully");
                break;

        case DREAMSOCK_MES_DISCONNECT:
                if(mClientVector.at(i) == NULL)
                        break;

                RemoveClient(mClientVector.at(i));

                LogString("LIBRARY: Server: a client disconnected");
                break;

        case DREAMSOCK_MES_PING:
                mClientVector.at(i)->SetPing(mNetwork->dreamSock_GetCurrentSystemTime() - mClientVector.at(i)->GetPingSent());
                break;
        }

}

int Server::CheckForTimeout(char *data, struct sockaddr *from)
{
	int currentTime = mNetwork->dreamSock_GetCurrentSystemTime();

	for (unsigned int i = 0; i < mClientVector.size(); i++)
	{
		// Don't timeout when connecting
		if(mClientVector.at(i)->GetConnectionState() == DREAMSOCK_CONNECTING)
		{
			continue;
		}

		// Check if the client has been silent for 30 seconds
		// If yes, assume crashed and remove the client
		if(currentTime - mClientVector.at(i)->GetLastMessageTime() > 30000)
		{
			LogString("Client timeout, disconnecting (%d - %d = %d)",
				currentTime, mClientVector.at(i)->GetLastMessageTime(), currentTime - mClientVector.at(i)->GetLastMessageTime());

			// Build a 'fake' message so the application will also
			// receive notification of a client disconnecting
			Message mes;
			mes.Init(data, sizeof(data));
			mes.WriteByte(DREAMSOCK_MES_DISCONNECT);

			*(struct sockaddr *) from = *mClientVector.at(i)->GetSocketAddress();

			RemoveClient(mClientVector.at(i));

			return mes.GetSize();
		}
	}
	return 0;
}

int Server::GetPacket(char *data, struct sockaddr *from)
{
	//LogString("getting packet");
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

	ret = mNetwork->dreamSock_GetPacket(socket, mes.data, from);

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

	for (unsigned int i = 0; i < mClientVector.size(); i++)
	{
		if(mClientVector.at(i)->mMessage.GetSize() == 0)
			continue;

		mClientVector.at(i)->SendPacket();
	}
}

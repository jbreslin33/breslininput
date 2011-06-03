
#include "server.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../network/network.h"
#include "../client/client.h"
#include "../message/message.h"
#include "../serverside/shape/shape.h"
#include "../serverside/game/game.h"
#include "../serverside/shape/shape.h"

Server::Server(Game* serverSideGame,const char *localIP, int serverPort)
{
	init			= false;
	mGame = serverSideGame;
	mLocalIP = localIP;
	port			= 0;
	
	runningIndex	= 1;

	// Store the server IP and port for later use
	port = serverPort;

	// Create server socket

	mNetwork = new Network(localIP, port);

	init = true;
}

Server::~Server()
{
	mClientVector.empty();
	mNetwork->dreamSock_CloseSocket(mNetwork->mSocket);
}

void Server::Uninitialise(void)
{
	mNetwork->dreamSock_CloseSocket(mNetwork->mSocket);

	init = false;
}

//when you get here to modify...you once again probably after the mClientVectorLoop than loop thru shapes.
//actually this should just be renamed sendAddNewShape because the internet client doesn not
//care what a serverClient is he only has a mShapeVector
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
			newClient->mMessage.WriteByte(1);	// local client
			newClient->mMessage.WriteByte(mClientVector.at(i)->GetIndex());
			newClient->mMessage.WriteString(mClientVector.at(i)->GetName());
		}
		else
		{
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
			continue;
		}
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

//called when internets client sends DREAMSOCK_MES_CONNECT message before it has a client, shape or anything.
void Server::AddClient(struct sockaddr *address, char *name)
{
	LogString("LIB: Adding client, index %d", runningIndex);
	Client* client = new Client(mNetwork);
	mClientVector.push_back(client);

	client->SetSocketAddress(address);
	client->SetConnectionState(DREAMSOCK_CONNECTING);
	client->SetOutgoingSequence(1);
	client->SetIncomingSequence(0);
	client->SetIncomingAcknowledged(0);
	client->SetIndex(runningIndex);
	client->SetName(name);

	memcpy(&client->myaddress,client->GetSocketAddress(), sizeof(struct sockaddr));

	mGame->createShape(client,runningIndex);

	runningIndex++;

	SendAddClient(client);
}

void Server::RemoveClient(Client *client)
{
	for (unsigned int i = 0; i < mClientVector.size(); i++)
	{
		if (mClientVector.at(i) == client)
		{
			mGame->RemoveShape(client->mShape);
			mClientVector.erase (mClientVector.begin()+i);
		}
	}
}



void Server::ParsePacket(Message *mes, struct sockaddr *address)
{
	mes->BeginReading();
    int type = mes->ReadByte();

	if (type == DREAMSOCK_MES_CONNECT)
	{
				AddClient(address, mes->ReadString());
				LogString("LIBRARY: Server: a client connected succesfully");
	}
	else
	{
		// Find the correct client by comparing addresses
		for (unsigned int i = 0; i < mClientVector.size(); i++)
		{
			if(memcmp(mClientVector.at(i)->GetSocketAddress(), address, sizeof(address)) == 0)
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
				{
					mClientVector.at(i)->SetConnectionState(DREAMSOCK_CONNECTED);
				}

				// Parse through the system messages
				switch(type)
				{
					case DREAMSOCK_MES_DISCONNECT:
						//if(mClientVector.at(i) == NULL)
						//	break;

					    RemoveClient(mClientVector.at(i));

						LogString("LIBRARY: Server: a client disconnected");
						break;

					case DREAMSOCK_MES_PING:
						mClientVector.at(i)->SetPing(mNetwork->dreamSock_GetCurrentSystemTime() - mClientVector.at(i)->GetPingSent());
						LogString("ping");
						break;
				}
			}
		}
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
	if(!mNetwork->mSocket)
		return 0;

	// Check for timeout
	int timeout = CheckForTimeout(data, from);

	if(timeout)
		return timeout;

	// Wait for a while or incoming data
	int maxfd = mNetwork->mSocket;
	fd_set allset;
	struct timeval waittime;

	waittime.tv_sec = 10 / 1000;
	waittime.tv_usec = (10 % 1000) * 1000;

	FD_ZERO(&allset); 
	FD_SET(mNetwork->mSocket, &allset);

	fd_set reading = allset;

	int nready = select(maxfd + 1, &reading, NULL, NULL, &waittime);

	if(!nready)
		return 0;

	// Read data of the socket
	int ret = 0;

	Message mes;
	mes.Init(data, sizeof(data));

	ret = mNetwork->dreamSock_GetPacket(mNetwork->mSocket, mes.data, from);

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
	if(!mNetwork->mSocket)
		return;

	for (unsigned int i = 0; i < mClientVector.size(); i++)
	{
		if(mClientVector.at(i)->mMessage.GetSize() == 0)
			continue;

		mClientVector.at(i)->SendPacket();
	}
}


void Server::ReadPackets(void)
{
	char data[1400];

	int type;
	int ret;

	struct sockaddr address;

	Message mes;
	mes.Init(data, sizeof(data));

	// Get the packet from the socket
	try
	{
		while(ret = GetPacket(mes.data, &address))
		{
			mes.SetSize(ret);
			mes.BeginReading();

			type = mes.ReadByte();

			// Check the type of the message
			switch(type)
			{
			case DREAMSOCK_MES_CONNECT:

				break;

			case DREAMSOCK_MES_DISCONNECT:

				for (unsigned int i = 0; i < mClientVector.size(); i++)
				{
					if(memcmp(&mClientVector.at(i)->myaddress, &address, sizeof(address)) == 0)
					{
						//mClientVector.erase(
					}
				}
				break;

			case USER_MES_FRAME:
//			LogString("Got frame (size: %d bytes)", ret);

				// Skip sequences
				mes.ReadShort();
				mes.ReadShort();

				for (unsigned int i = 0; i < mClientVector.size(); i++)
				{
					if(memcmp(&mClientVector.at(i)->myaddress, &address, sizeof(address)) == 0)
					{
						mGame->ReadDeltaMoveCommand(&mes, mClientVector.at(i));
						mClientVector.at(i)->mShape->processTick();

						break;
					}
				}

				break;

			case USER_MES_NONDELTAFRAME:

				for (unsigned int i = 0; i < mClientVector.size(); i++)
				{
					mClientVector.at(i)->mMessage.Init(mClientVector.at(i)->mMessage.outgoingData,
						sizeof(mClientVector.at(i)->mMessage.outgoingData));

					mClientVector.at(i)->mMessage.WriteByte(USER_MES_NONDELTAFRAME);
					mClientVector.at(i)->mMessage.WriteShort(mClientVector.at(i)->GetOutgoingSequence());
					mClientVector.at(i)->mMessage.WriteShort(mClientVector.at(i)->GetIncomingSequence());

					for (unsigned int j = 0; j < mGame->mShapeVector.size(); j++)
					{
						mGame->BuildMoveCommand(&mClientVector.at(i)->mMessage, mGame->mShapeVector.at(j));
					}
					mClientVector.at(i)->SendPacket();
				}

				break;

			}
		}
	}
	catch(...)
	{
		LogString("Unknown Exception caught in Lobby ReadPackets loop");

#ifdef WIN32
		MessageBox(NULL, "Unknown Exception caught in Lobby ReadPackets loop", "Error", MB_OK | MB_TASKMODAL);
#endif
	}
}

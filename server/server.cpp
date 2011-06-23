
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
	
	// Store the server IP and port for later use
	port = serverPort;

	// Create network
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

//send a shape that has a client. i.e. a new human player
void Server::SendAddShape(Client* client)
{

	// init mMessage for client
	client->mMessage.Init(client->mMessage.outgoingData,
		sizeof(client->mMessage.outgoingData));

	//this a new client so let him change to connectionState = DREAMSOCK_CONNECTED; 
	client->mMessage.WriteByte(DREAMSOCK_MES_CONNECT);	// type
	client->SendPacket();

	// First inform the new client of the other shapes by looping thru entire
	//mShapeVector one by one and sending out client->SendPacket();
	for (unsigned int i = 0; i < mGame->mShapeVector.size(); i++)
	{
		//init mMessage for client
		client->mMessage.Init(client->mMessage.outgoingData,
			sizeof(client->mMessage.outgoingData));

		client->mMessage.WriteByte(DREAMSOCK_MES_ADDSHAPE); // tell internet clients to add a shape

		if (mGame->mShapeVector.at(i) == client->mShape)
		{
			client->mMessage.WriteByte(1);	// local shape
			client->mMessage.WriteByte(mGame->mShapeVector.at(i)->mIndex);
			client->mMessage.WriteString("");
		}
		else
		{
			client->mMessage.WriteByte(0);	// not-local shape
			client->mMessage.WriteByte(mGame->mShapeVector.at(i)->mIndex);
			client->mMessage.WriteString("");
		}
		client->mMessage.WriteFloat(mGame->mShapeVector.at(i)->mCommand.mOrigin.x);
		client->mMessage.WriteFloat(mGame->mShapeVector.at(i)->mCommand.mOrigin.z);
		client->mMessage.WriteFloat(mGame->mShapeVector.at(i)->mCommand.mOrigin.y);

		client->mMessage.WriteFloat(mGame->mShapeVector.at(i)->mCommand.mVelocity.x);
		client->mMessage.WriteFloat(mGame->mShapeVector.at(i)->mCommand.mVelocity.z);
		client->mMessage.WriteFloat(mGame->mShapeVector.at(i)->mCommand.mVelocity.y);

		client->mMessage.WriteFloat(mGame->mShapeVector.at(i)->mCommand.mRot.x);
		client->mMessage.WriteFloat(mGame->mShapeVector.at(i)->mCommand.mRot.z);
	

		client->SendPacket(&client->mMessage);
	}

	// Then tell the others about the new shape
	for (unsigned int i = 0; i < mClientVector.size(); i++)
	{
		//don't send to the client you allready sent to above...
		if(mClientVector.at(i) == client)
		{
			continue;
		}
		else //inform everyone else
		{
			//init mMessage for client
			mClientVector.at(i)->mMessage.Init(mClientVector.at(i)->mMessage.outgoingData,
				sizeof(mClientVector.at(i)->mMessage.outgoingData));

			mClientVector.at(i)->mMessage.WriteByte(DREAMSOCK_MES_ADDSHAPE); // type
			mClientVector.at(i)->mMessage.WriteByte(0);
			mClientVector.at(i)->mMessage.WriteByte(client->mShape->mIndex);
			mClientVector.at(i)->mMessage.WriteString("");
			
			mClientVector.at(i)->mMessage.WriteFloat(client->mShape->mCommand.mOrigin.x);
			mClientVector.at(i)->mMessage.WriteFloat(client->mShape->mCommand.mOrigin.z);
			mClientVector.at(i)->mMessage.WriteFloat(client->mShape->mCommand.mOrigin.y);

			mClientVector.at(i)->mMessage.WriteFloat(client->mShape->mCommand.mVelocity.x);
			mClientVector.at(i)->mMessage.WriteFloat(client->mShape->mCommand.mVelocity.z);
			mClientVector.at(i)->mMessage.WriteFloat(client->mShape->mCommand.mVelocity.y);

			mClientVector.at(i)->mMessage.WriteFloat(client->mShape->mCommand.mRot.x);
			mClientVector.at(i)->mMessage.WriteFloat(client->mShape->mCommand.mRot.z);
		

			mClientVector.at(i)->SendPacket();
		}
	}
}

//this is your serverside guy. he has no client, but we still need to tell everyone about the chap
void Server::SendAddAIShape(Shape* shape)
{
	// Send 'Add Shape' message to every client
	// Then tell the others about the new shape
	for (unsigned int i = 0; i < mClientVector.size(); i++)
	{
		//init mMessage for client
		mClientVector.at(i)->mMessage.Init(mClientVector.at(i)->mMessage.outgoingData,
			sizeof(mClientVector.at(i)->mMessage.outgoingData));

		mClientVector.at(i)->mMessage.WriteByte(DREAMSOCK_MES_ADDSHAPE); // type
		mClientVector.at(i)->mMessage.WriteByte(0);
		mClientVector.at(i)->mMessage.WriteByte(shape->mIndex);
		mClientVector.at(i)->mMessage.WriteString("");

		mClientVector.at(i)->mMessage.WriteFloat(shape->mCommand.mOrigin.x);
		mClientVector.at(i)->mMessage.WriteFloat(shape->mCommand.mOrigin.z);
		mClientVector.at(i)->mMessage.WriteFloat(shape->mCommand.mOrigin.y);

		mClientVector.at(i)->mMessage.WriteFloat(shape->mCommand.mVelocity.x);
		mClientVector.at(i)->mMessage.WriteFloat(shape->mCommand.mVelocity.z);
		mClientVector.at(i)->mMessage.WriteFloat(shape->mCommand.mVelocity.y);

		mClientVector.at(i)->mMessage.WriteFloat(shape->mCommand.mRot.x);
		mClientVector.at(i)->mMessage.WriteFloat(shape->mCommand.mRot.z);


		mClientVector.at(i)->SendPacket();
	}
}

void Server::SendRemoveShape(Shape* shape)
{
	
	int index = shape->mIndex;

	// Send 'DREAMSOCK_MES_REMOVESHAPE' message to every client
	for (unsigned int i = 0; i < mClientVector.size(); i++)
	{
		mClientVector.at(i)->mMessage.Init(mClientVector.at(i)->mMessage.outgoingData,
			sizeof(mClientVector.at(i)->mMessage.outgoingData));

		mClientVector.at(i)->mMessage.WriteByte(DREAMSOCK_MES_REMOVESHAPE);	// type
		mClientVector.at(i)->mMessage.WriteByte(index);							// index
	}

	SendPackets();
	
	//do i need this next bit?
/*
	// Send disconnection confirmation
	client->mMessage.Init(shape->mMessage.outgoingData,
		sizeof(shape->mMessage.outgoingData));

	client->mMessage.WriteByte(DREAMSOCK_MES_DISCONNECT);
	client->SendPacket();
*/
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
	Client* client = new Client(mNetwork);
	mClientVector.push_back(client);

	client->SetSocketAddress(address);
	client->SetConnectionState(DREAMSOCK_CONNECTING);
	client->SetOutgoingSequence(1);
	client->SetIncomingSequence(0);
	client->SetIncomingAcknowledged(0);
	client->SetName(name);

	memcpy(&client->myaddress,client->GetSocketAddress(), sizeof(struct sockaddr));

	mGame->createShape(client);

	LogString("LIB: Adding client with shape index %d", client->mShape->mIndex);

	SendAddShape(client);  
}

void Server::RemoveClient(Client *client)
{
	for (unsigned int i = 0; i < mClientVector.size(); i++)
	{
		if (mClientVector.at(i) == client)
		{
			mGame->RemoveShape(client->mShape); //remove the shape associated with this client while your at it.
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
				
				//let's try this with shapes instead.....
				for (unsigned int i = 0; i < mGame->mShapeVector.size(); i++)
				{
					if (mGame->mShapeVector.at(i)->mClient != NULL)
					{
						if(memcmp(&mGame->mShapeVector.at(i)->mClient->myaddress, &address, sizeof(address)) == 0)
						{
							mGame->ReadDeltaMoveCommand(&mes, mGame->mShapeVector.at(i)->mClient);
							mGame->mShapeVector.at(i)->processTick();

							break;
						}
					}
					
					//else
					//{
						//just processtick for ai guys because their moves come from ai class/states
					//	mGame->mShapeVector.at(i)->processTick();
					//}
					
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

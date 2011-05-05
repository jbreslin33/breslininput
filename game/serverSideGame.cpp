#include "serverSideGame.h"

#include "../shape/shape.h"
#include "../client/serverSideClient.h"
#include "../player/serverSidePlayer.h"
#include <fstream>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#include <assert.h>
#endif

float VectorLength(Vector3D *vec)
{
	return (float) sqrt(vec->x*vec->x + vec->z*vec->z);
}

Vector3D VectorSubstract(Vector3D *vec1, Vector3D *vec2)
{
	Vector3D vec;

	vec.x = vec1->x - vec2->x;
	vec.z = vec1->z - vec2->z;

	return vec;
}

ServerSideGame::ServerSideGame()
{
	mNetworkServer = new dreamServer;
	mRealTime	= 0;
	mServerTime	= 0;
	mFramenum	= 0;
}

ServerSideGame::~ServerSideGame()
{
	delete mNetworkServer;
}

int ServerSideGame::InitNetwork()
{
	if(dreamSock_Initialize() != 0)
	{
		LogString("Error initialising Communication Library!");
		return 1;
	}

	LogString("Initialising game");

	// Create the game servers on new ports, starting from 30004
	int ret = mNetworkServer->Initialise("", 30004);

	if(ret == DREAMSOCK_SERVER_ERROR)
	{
#ifdef WIN32
		char text[64];
		sprintf(text, "Could not open server on port %d", mNetworkServer->GetPort());

		MessageBox(NULL, text, "Error", MB_OK);
#else
		LogString("Could not open server on port %d", mNetworkServer->GetPort());
#endif
		return 1;
	}

	return 0;
}

void ServerSideGame::ShutdownNetwork(void)
{
	RemoveClients();
	mNetworkServer->Uninitialise();
}

void ServerSideGame::AddClient(void)
{
	dreamClient *netList = mNetworkServer->GetClientList();
	
	if (mClientVector.size() == 0)
	{
		LogString("about to create player");

		ServerSideClient* serverSideClient = new ServerSideClient();
		serverSideClient->netClient = netList;
		memcpy(&serverSideClient->address,serverSideClient->netClient->GetSocketAddress(), sizeof(struct sockaddr));

		mClientVector.push_back(serverSideClient);
	
		serverSideClient->mPlayer = new ServerSidePlayer("jay",serverSideClient);
		netList->next = NULL;
	}
	else
	{
		dreamClient* netnext = mClientVector.back()->netClient->next;
		ServerSideClient* serverSideClient = new ServerSideClient();

		serverSideClient->netClient = netnext;
		memcpy(&serverSideClient->address,serverSideClient->netClient->GetSocketAddress(), sizeof(struct sockaddr));

		mClientVector.push_back(serverSideClient);

		serverSideClient->mPlayer = new ServerSidePlayer("jay",serverSideClient);
	}
}

void ServerSideGame::RemoveClient(ServerSideClient* client)
{
}

void ServerSideGame::RemoveClients(void)
{
	mClientVector.empty();
}

void ServerSideGame::Frame(int msec)
{
	mRealTime += msec;

	// Read packets from clients
	ReadPackets();

	// Wait full 100 ms before allowing to send
	if(mRealTime < mServerTime)
	{
		// never let the time get too far off
		if(mServerTime - mRealTime > 32)
		{
			mRealTime = mServerTime - 32;
		}

		return;
	}

	// Bump frame number, and calculate new mServerTime
	mFramenum++;
	mServerTime = mFramenum * 32;

	if(mServerTime < mRealTime)
		mRealTime = mServerTime;

	SendCommand();
}

void ServerSideGame::ReadPackets(void)
{
	char data[1400];

	int type;
	int ret;

	struct sockaddr address;

	ServerSideClient *clList;

	dreamMessage mes;
	mes.Init(data, sizeof(data));

	// Get the packet from the socket
	try
	{
		while(ret = mNetworkServer->GetPacket(mes.data, &address))
		{
			mes.SetSize(ret);
			mes.BeginReading();

			type = mes.ReadByte();

			// Check the type of the message
			switch(type)
			{
			case DREAMSOCK_MES_CONNECT:
				AddClient();
				break;

			case DREAMSOCK_MES_DISCONNECT:

				for (int i = 0; i < mClientVector.size(); i++)
				{
					if(memcmp(&mClientVector.at(i)->address, &address, sizeof(address)) == 0)
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

				for (int i = 0; i < mClientVector.size(); i++)
				{
					if(memcmp(&mClientVector.at(i)->address, &address, sizeof(address)) == 0)
					{
						ReadDeltaMoveCommand(&mes, mClientVector.at(i));
						mClientVector.at(i)->mPlayer->processTick();

						break;
					}
				}

				break;

			case USER_MES_NONDELTAFRAME:

				for (int i = 0; i < mClientVector.size(); i++)
				{
					mClientVector.at(i)->netClient->message.Init(mClientVector.at(i)->netClient->message.outgoingData,
						sizeof(clList->netClient->message.outgoingData));

					mClientVector.at(i)->netClient->message.WriteByte(USER_MES_NONDELTAFRAME);
					mClientVector.at(i)->netClient->message.WriteShort(mClientVector.at(i)->netClient->GetOutgoingSequence());
					mClientVector.at(i)->netClient->message.WriteShort(mClientVector.at(i)->netClient->GetIncomingSequence());

					for (int j = 0; j < mClientVector.size(); j++)
					{
						BuildMoveCommand(&mClientVector.at(i)->netClient->message, mClientVector.at(j));
					}
					mClientVector.at(i)->netClient->SendPacket();
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

void ServerSideGame::SendCommand(void)
{
	// Fill messages
	for (int i = 0; i < mClientVector.size(); i++)
	{
		mClientVector.at(i)->netClient->message.Init(mClientVector.at(i)->netClient->message.outgoingData,
			sizeof(mClientVector.at(i)->netClient->message.outgoingData));

		mClientVector.at(i)->netClient->message.WriteByte(USER_MES_FRAME);			// type
		mClientVector.at(i)->netClient->message.AddSequences(mClientVector.at(i)->netClient);	// sequences

		for (int j = 0; j < mClientVector.size(); j++)
		{
			BuildDeltaMoveCommand(&mClientVector.at(i)->netClient->message, mClientVector.at(j));
		}
	}

	// Send messages to all clients
	mNetworkServer->SendPackets();

	// Store the sent command in history
	for (int i = 0; i < mClientVector.size(); i++)
	{
		int num = (mClientVector.at(i)->netClient->GetOutgoingSequence() - 1) & (COMMAND_HISTORY_SIZE-1);
		memcpy(&mClientVector.at(i)->mFrame[num], &mClientVector.at(i)->mCommand, sizeof(ServerSideCommand));
	}
}

void ServerSideGame::SendExitNotification(void)
{
	for (int i = 0; i < mClientVector.size(); i++)
	{
		mClientVector.at(i)->netClient->message.Init(mClientVector.at(i)->netClient->message.outgoingData,
			sizeof(mClientVector.at(i)->netClient->message.outgoingData));

		mClientVector.at(i)->netClient->message.WriteByte(USER_MES_SERVEREXIT);	// type
		mClientVector.at(i)->netClient->message.AddSequences(mClientVector.at(i)->netClient);	// sequences
	}

	mNetworkServer->SendPackets();
}

void ServerSideGame::ReadDeltaMoveCommand(dreamMessage *mes, ServerSideClient *client)
{
	int flags = 0;

	// Flags
	flags = mes->ReadByte();

	// Key
	if(flags & CMD_KEY)
	{
		client->mCommand.mKey = mes->ReadByte();

		LogString("Client %d: read CMD_KEY (%d)", client->netClient->GetIndex(), client->mCommand.mKey);
	}

	// Read time to run command
	client->mCommand.mMilliseconds = mes->ReadByte();
}

void ServerSideGame::BuildMoveCommand(dreamMessage *mes, ServerSideClient *client)
{
	Command* command = &client->mCommand;
	// Add to the message
	// Key
	mes->WriteByte(command->mKey);

	// Origin
	mes->WriteFloat(command->mOrigin.x);
	mes->WriteFloat(command->mOrigin.z);
	mes->WriteFloat(command->mVelocity.x);
	mes->WriteFloat(command->mVelocity.z);

	mes->WriteByte(command->mMilliseconds);
}

void ServerSideGame::BuildDeltaMoveCommand(dreamMessage *mes, ServerSideClient *client)
{

	ServerSidePlayer* player = client->mPlayer;
	Command* command = &client->mCommand;
	int flags = 0;

	int last = (client->netClient->GetOutgoingSequence() - 1) & (COMMAND_HISTORY_SIZE-1);

	// Check what needs to be updated
	if(client->mFrame[last].mKey != command->mKey)
	{
		flags |= CMD_KEY;
	}

	if(client->mFrame[last].mOrigin.x != command->mOrigin.x ||
		client->mFrame[last].mOrigin.z != command->mOrigin.z)
	{
		flags |= CMD_ORIGIN;
	}

	// Add to the message
	// Flags
	mes->WriteByte(flags);

	// Key
	if(flags & CMD_KEY)
	{
		mes->WriteByte(command->mKey);
	}

	// Origin
	if(flags & CMD_ORIGIN)
	{
		mes->WriteByte(client->mProcessedFrame & (COMMAND_HISTORY_SIZE-1));
	}

	mes->WriteFloat(command->mOrigin.x);
	mes->WriteFloat(command->mOrigin.z);

	mes->WriteFloat(command->mVelocity.x);
	mes->WriteFloat(command->mVelocity.z);

	mes->WriteByte(command->mMilliseconds);
}

#include "serverSideGame.h"
#include "../server/server.h"
#include "../shape/ogreShape.h"
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

ServerSideGame::ServerSideGame()
{

#ifdef _DEBUG
	mRoot = new Ogre::Root("plugins_d.cfg");
#else
	mRoot = new Ogre::Root("plugins.cfg");
#endif

	mNetworkServer = new Server(this);
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
	/*
	dreamClient *netList = mNetworkServer->GetClientList();
	
	if (mClientVector.size() == 0)
	{
		LogString("about to create player");

		ServerSideClient* serverSideClient = new ServerSideClient();
		serverSideClient->netClient = netList;
		memcpy(&serverSideClient->address,serverSideClient->netClient->GetSocketAddress(), sizeof(struct sockaddr));

		mClientVector.push_back(serverSideClient);
	
		OgreShape* shape = new OgreShape("jay" + mClientVector.size(),new Vector3D(),mRoot);
		serverSideClient->mPlayer = new ServerSidePlayer("jay" + mClientVector.size(),serverSideClient,shape);
		netList->next = NULL;
	}
	else
	{
		dreamClient* netnext = mClientVector.back()->netClient->next;
		ServerSideClient* serverSideClient = new ServerSideClient();

		serverSideClient->netClient = netnext;
		memcpy(&serverSideClient->address,serverSideClient->netClient->GetSocketAddress(), sizeof(struct sockaddr));

		mClientVector.push_back(serverSideClient);

		OgreShape* shape = new OgreShape("jay" + mClientVector.size(),new Vector3D(),mRoot);
		serverSideClient->mPlayer = new ServerSidePlayer("jay" + mClientVector.size(),serverSideClient,shape);
	}
	*/
}

void ServerSideGame::RemoveClient(Client* client)
{
}

void ServerSideGame::RemoveClients(void)
{

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

	//LogString("REading packetsdfdfdfdd");
	char data[1400];

	int type;
	int ret;

	struct sockaddr address;

	Message mes;
	mes.Init(data, sizeof(data));

	// Get the packet from the socket
	try
	{
		//LogString("trying.....");
		while(ret = mNetworkServer->GetPacket(mes.data, &address))
		{
			mes.SetSize(ret);
			mes.BeginReading();

			type = mes.ReadByte();
			//LogString("type:%s",type);
			// Check the type of the message
			switch(type)
			{
			case DREAMSOCK_MES_CONNECT:
				AddClient();
				break;

			case DREAMSOCK_MES_DISCONNECT:

				for (unsigned int i = 0; i < mNetworkServer->mClientVector.size(); i++)
				{
					if(memcmp(&mNetworkServer->mClientVector.at(i)->myaddress, &address, sizeof(address)) == 0)
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

				for (unsigned int i = 0; i < mNetworkServer->mClientVector.size(); i++)
				{
					if(memcmp(&mNetworkServer->mClientVector.at(i)->myaddress, &address, sizeof(address)) == 0)
					{
						ReadDeltaMoveCommand(&mes, mNetworkServer->mClientVector.at(i));
						mNetworkServer->mClientVector.at(i)->mServerSidePlayer->processTick();

						break;
					}
				}

				break;

			case USER_MES_NONDELTAFRAME:

				for (unsigned int i = 0; i < mNetworkServer->mClientVector.size(); i++)
				{
					mNetworkServer->mClientVector.at(i)->mMessage.Init(mNetworkServer->mClientVector.at(i)->mMessage.outgoingData,
						sizeof(mNetworkServer->mClientVector.at(i)->mMessage.outgoingData));

					mNetworkServer->mClientVector.at(i)->mMessage.WriteByte(USER_MES_NONDELTAFRAME);
					mNetworkServer->mClientVector.at(i)->mMessage.WriteShort(mNetworkServer->mClientVector.at(i)->GetOutgoingSequence());
					mNetworkServer->mClientVector.at(i)->mMessage.WriteShort(mNetworkServer->mClientVector.at(i)->GetIncomingSequence());

					for (unsigned int j = 0; j < mNetworkServer->mClientVector.size(); j++)
					{
						BuildMoveCommand(&mNetworkServer->mClientVector.at(i)->mMessage, mNetworkServer->mClientVector.at(j));
					}
					mNetworkServer->mClientVector.at(i)->SendPacket();
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
	for (unsigned int i = 0; i < mNetworkServer->mClientVector.size(); i++)
	{
		mNetworkServer->mClientVector.at(i)->mMessage.Init(mNetworkServer->mClientVector.at(i)->mMessage.outgoingData,
			sizeof(mNetworkServer->mClientVector.at(i)->mMessage.outgoingData));

		mNetworkServer->mClientVector.at(i)->mMessage.WriteByte(USER_MES_FRAME);			// type
		mNetworkServer->mClientVector.at(i)->mMessage.AddSequences(mNetworkServer->mClientVector.at(i));	// sequences

		for (unsigned int j = 0; j < mNetworkServer->mClientVector.size(); j++)
		{
			BuildDeltaMoveCommand(&mNetworkServer->mClientVector.at(i)->mMessage, mNetworkServer->mClientVector.at(j));
		}
	}

	// Send messages to all clients
	mNetworkServer->SendPackets();

	// Store the sent command in history
	for (unsigned int i = 0; i < mNetworkServer->mClientVector.size(); i++)
	{
		int num = (mNetworkServer->mClientVector.at(i)->GetOutgoingSequence() - 1) & (COMMAND_HISTORY_SIZE-1);
		memcpy(&mNetworkServer->mClientVector.at(i)->mServerSidePlayer->mFrame[num], &mNetworkServer->mClientVector.at(i)->mServerSidePlayer->mCommand, sizeof(ServerSideCommand));
	}
}

void ServerSideGame::SendExitNotification(void)
{
	for (unsigned int i = 0; i < mNetworkServer->mClientVector.size(); i++)
	{
		mNetworkServer->mClientVector.at(i)->mMessage.Init(mNetworkServer->mClientVector.at(i)->mMessage.outgoingData,
			sizeof(mNetworkServer->mClientVector.at(i)->mMessage.outgoingData));

		mNetworkServer->mClientVector.at(i)->mMessage.WriteByte(USER_MES_SERVEREXIT);	// type
		mNetworkServer->mClientVector.at(i)->mMessage.AddSequences(mNetworkServer->mClientVector.at(i));	// sequences
	}

	mNetworkServer->SendPackets();
}

void ServerSideGame::ReadDeltaMoveCommand(Message *mes, Client *client)
{
	int flags = 0;

	// Flags
	flags = mes->ReadByte();

	// Key
	if(flags & CMD_KEY)
	{
		client->mServerSidePlayer->mCommand.mKey = mes->ReadByte();

		//LogString("Client %d: read CMD_KEY (%d)", client->netClient->GetIndex(), client->mCommand.mKey);
	}

	// Read time to run command
	client->mServerSidePlayer->mCommand.mMilliseconds = mes->ReadByte();
}

void ServerSideGame::BuildMoveCommand(Message *mes, Client *client)
{
	Command* command = &client->mServerSidePlayer->mCommand;
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

void ServerSideGame::BuildDeltaMoveCommand(Message *mes, Client *client)
{

	ServerSidePlayer* player = client->mServerSidePlayer;
	Command* command = &client->mServerSidePlayer->mCommand;
	int flags = 0;

	int last = (client->GetOutgoingSequence() - 1) & (COMMAND_HISTORY_SIZE-1);

	// Check what needs to be updated
	if(client->mServerSidePlayer->mFrame[last].mKey != command->mKey)
	{
		flags |= CMD_KEY;
	}

	if(client->mServerSidePlayer->mFrame[last].mOrigin.x != command->mOrigin.x ||
		client->mServerSidePlayer->mFrame[last].mOrigin.z != command->mOrigin.z)
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
		mes->WriteByte(client->mServerSidePlayer->mProcessedFrame & (COMMAND_HISTORY_SIZE-1));
	}

	mes->WriteFloat(command->mOrigin.x);
	mes->WriteFloat(command->mOrigin.z);

	mes->WriteFloat(command->mVelocity.x);
	mes->WriteFloat(command->mVelocity.z);

    mes->WriteFloat(command->mRot.x);
	mes->WriteFloat(command->mRot.z);


	mes->WriteByte(command->mMilliseconds);
}

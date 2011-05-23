#include "serverSideGame.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../network/network.h"

#include "../server/server.h"

#include "../shape/ogreShape.h"
#include "../client/serverSideClient.h"
#include "../player/serverSidePlayer.h"
#include "../command/serverSideCommand.h"

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
	StartLog();
	mNetwork = new Network();

#ifdef _DEBUG
	mRoot = new Ogre::Root("plugins_d.cfg");
#else
	mRoot = new Ogre::Root("plugins.cfg");
#endif

	mServer = new Server(this,"", 30004);
	mRealTime	= 0;
	mServerTime	= 0;
	mFramenum	= 0;
}

ServerSideGame::~ServerSideGame()
{
	delete mServer;
}
/*
int ServerSideGame::InitNetwork()
{
	// Create the game servers on new ports, starting from 30004
	int ret = mServer->Initialise("", 30004);

	if(ret == DREAMSOCK_SERVER_ERROR)
	{
#ifdef WIN32
		char text[64];
		sprintf(text, "Could not open server on port %d", mServer->GetPort());

		MessageBox(NULL, text, "Error", MB_OK);
#else
		LogString("Could not open server on port %d", mServer->GetPort());
#endif
		return 1;
	}

	return 0;
}
*/
void ServerSideGame::ShutdownNetwork(void)
{
	RemoveClients();
	mServer->Uninitialise();
}

void ServerSideGame::createPlayer(Client* client, int runningIndex)
{
	OgreShape* shape = new OgreShape("oshape" + runningIndex,new Vector3D(),mRoot);
	client->mServerSidePlayer = new ServerSidePlayer("oplayer" + runningIndex,client,shape);
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
		while(ret = mServer->GetPacket(mes.data, &address))
		{
			mes.SetSize(ret);
			mes.BeginReading();

			type = mes.ReadByte();
			//LogString("type:%s",type);
			// Check the type of the message
			switch(type)
			{
			case DREAMSOCK_MES_CONNECT:

				break;

			case DREAMSOCK_MES_DISCONNECT:

				for (unsigned int i = 0; i < mServer->mClientVector.size(); i++)
				{
					if(memcmp(&mServer->mClientVector.at(i)->myaddress, &address, sizeof(address)) == 0)
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

				for (unsigned int i = 0; i < mServer->mClientVector.size(); i++)
				{
					if(memcmp(&mServer->mClientVector.at(i)->myaddress, &address, sizeof(address)) == 0)
					{
						ReadDeltaMoveCommand(&mes, mServer->mClientVector.at(i));
						mServer->mClientVector.at(i)->mServerSidePlayer->processTick();

						break;
					}
				}

				break;

			case USER_MES_NONDELTAFRAME:

				for (unsigned int i = 0; i < mServer->mClientVector.size(); i++)
				{
					mServer->mClientVector.at(i)->mMessage.Init(mServer->mClientVector.at(i)->mMessage.outgoingData,
						sizeof(mServer->mClientVector.at(i)->mMessage.outgoingData));

					mServer->mClientVector.at(i)->mMessage.WriteByte(USER_MES_NONDELTAFRAME);
					mServer->mClientVector.at(i)->mMessage.WriteShort(mServer->mClientVector.at(i)->GetOutgoingSequence());
					mServer->mClientVector.at(i)->mMessage.WriteShort(mServer->mClientVector.at(i)->GetIncomingSequence());

					for (unsigned int j = 0; j < mServer->mClientVector.size(); j++)
					{
						BuildMoveCommand(&mServer->mClientVector.at(i)->mMessage, mServer->mClientVector.at(j));
					}
					mServer->mClientVector.at(i)->SendPacket();
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
	for (unsigned int i = 0; i < mServer->mClientVector.size(); i++)
	{
		mServer->mClientVector.at(i)->mMessage.Init(mServer->mClientVector.at(i)->mMessage.outgoingData,
			sizeof(mServer->mClientVector.at(i)->mMessage.outgoingData));

		mServer->mClientVector.at(i)->mMessage.WriteByte(USER_MES_FRAME);			// type
		mServer->mClientVector.at(i)->mMessage.AddSequences(mServer->mClientVector.at(i));	// sequences

		for (unsigned int j = 0; j < mServer->mClientVector.size(); j++)
		{
			BuildDeltaMoveCommand(&mServer->mClientVector.at(i)->mMessage, mServer->mClientVector.at(j));
		}
	}

	// Send messages to all clients
	mServer->SendPackets();

	// Store the sent command in history
	for (unsigned int i = 0; i < mServer->mClientVector.size(); i++)
	{
		int num = (mServer->mClientVector.at(i)->GetOutgoingSequence() - 1) & (COMMAND_HISTORY_SIZE-1);
		memcpy(&mServer->mClientVector.at(i)->mServerSidePlayer->mFrame[num], &mServer->mClientVector.at(i)->mServerSidePlayer->mCommand, sizeof(ServerSideCommand));
	}
}

void ServerSideGame::SendExitNotification(void)
{
	for (unsigned int i = 0; i < mServer->mClientVector.size(); i++)
	{
		mServer->mClientVector.at(i)->mMessage.Init(mServer->mClientVector.at(i)->mMessage.outgoingData,
			sizeof(mServer->mClientVector.at(i)->mMessage.outgoingData));

		mServer->mClientVector.at(i)->mMessage.WriteByte(USER_MES_SERVEREXIT);	// type
		mServer->mClientVector.at(i)->mMessage.AddSequences(mServer->mClientVector.at(i));	// sequences
	}

	mServer->SendPackets();
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

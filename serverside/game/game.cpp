#include "game.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../server/server.h"

#include "../../client/serverSideClient.h"
#include "../../serverside/shape/shape.h"
#include "../../command/command.h"

Game::Game()
{
	StartLog();

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

Game::~Game()
{
	StopLog();
	delete mServer;
}

void Game::ShutdownNetwork(void)
{
	mServer->Uninitialise();
}

void Game::createPlayer(Client* client, int runningIndex)
{
	client->mShape = new Shape("oplayer" + runningIndex, new Vector3D(),mRoot);
	client->mShape->mClient = client;
}

void Game::Frame(int msec)
{
	mRealTime += msec;

	// Read packets from clients
	mServer->ReadPackets();

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

void Game::SendCommand(void)
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
		memcpy(&mServer->mClientVector.at(i)->mShape->mFrame[num], &mServer->mClientVector.at(i)->mShape->mCommand, sizeof(Command));
	}
}

void Game::SendExitNotification(void)
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

void Game::ReadDeltaMoveCommand(Message *mes, Client *client)
{
	int flags = 0;

	// Flags
	flags = mes->ReadByte();

	// Key
	if(flags & CMD_KEY)
	{
		client->mShape->mCommand.mKey = mes->ReadByte();

		//LogString("Client %d: read CMD_KEY (%d)", client->netClient->GetIndex(), client->mCommand.mKey);
	}

	// Read time to run command
	client->mShape->mCommand.mMilliseconds = mes->ReadByte();
	//let's set the shape's clientFrameTime right here.....
	client->mShape->mCommand.mClientFrametime = client->mShape->mCommand.mMilliseconds / 1000.0f;
}

void Game::BuildMoveCommand(Message *mes, Client *client)
{
	Command* command = &client->mShape->mCommand;
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

void Game::BuildDeltaMoveCommand(Message *mes, Client *client)
{

	Shape* player = client->mShape;
	Command* command = &client->mShape->mCommand;
	int flags = 0;

	int last = (client->GetOutgoingSequence() - 1) & (COMMAND_HISTORY_SIZE-1);

	// Check what needs to be updated
	if(client->mShape->mFrame[last].mKey != command->mKey)
	{
		flags |= CMD_KEY;
	}

	if(client->mShape->mFrame[last].mOrigin.x != command->mOrigin.x ||
		client->mShape->mFrame[last].mOrigin.z != command->mOrigin.z)
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
		mes->WriteByte(client->mShape->mProcessedFrame & (COMMAND_HISTORY_SIZE-1));
	}

	mes->WriteFloat(command->mOrigin.x);
	mes->WriteFloat(command->mOrigin.z);
	mes->WriteFloat(command->mOrigin.y);

	mes->WriteFloat(command->mVelocity.x);
	mes->WriteFloat(command->mVelocity.z);
	mes->WriteFloat(command->mVelocity.y);

    mes->WriteFloat(command->mRot.x);
	mes->WriteFloat(command->mRot.z);


	mes->WriteByte(command->mMilliseconds);
}

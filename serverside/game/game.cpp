#include "game.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../server/server.h"
#include "../../network/network.h"

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

	mRunningShapeIndex = 1;
	
	for(int i = 0; i < 20; i++)
	{
		createAIShape();
	}
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

void Game::createShape(Client* client)
{

	Vector3D* pos = new Vector3D();
	pos->x = 0;
	pos->y = 0;
	pos->z = 0;

	Shape* shape = new Shape(pos,mRoot); 
	shape->mGame = this; //for now to give access to shapeVector for collision i guess
	shape->mIndex = mRunningShapeIndex;
	mShapeVector.push_back(shape); //either way add this to shape vector
	mRunningShapeIndex++;

	if (client != NULL)
	{
		client->mShape = shape; 
		client->mShape->mClient = client; //set client it could be NULL meaning just a serverside shape
	}
}

void Game::createAIShape()
{
	Vector3D* pos = new Vector3D();
	pos->x = 0;
	pos->y = 0;
	pos->z = 0;

	Shape* shape = new Shape(pos,mRoot); 
	shape->mGame = this; //for now to give access to shapeVector for collision i guess
	shape->mIndex = mRunningShapeIndex;
	mShapeVector.push_back(shape); //either way add this to shape vector
	mRunningShapeIndex++;

	mServer->SendAddAIShape(shape);
}

void Game::RemoveShape(Shape* shape)
{
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		if (mShapeVector.at(i) == shape)
		{
			mServer->SendRemoveShape(shape);
			mShapeVector.erase (mShapeVector.begin()+i);
		}
	}
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

//send to updates to all clients about all shapes
void Game::SendCommand(void)
{
	// Fill messages..for all clients
	for (unsigned int i = 0; i < mServer->mClientVector.size(); i++)
	{
		//standard initialize of mMessage for client in this case
		mServer->mClientVector.at(i)->mMessage.Init(mServer->mClientVector.at(i)->mMessage.outgoingData,
			sizeof(mServer->mClientVector.at(i)->mMessage.outgoingData));

		//start filling said mMessage that belongs to client
		mServer->mClientVector.at(i)->mMessage.WriteByte(USER_MES_FRAME);			// type
		mServer->mClientVector.at(i)->mMessage.AddSequences(mServer->mClientVector.at(i));	// sequences

		//this is where you need to actually loop thru the shapes not the clients but put write to client mMessage
		for (unsigned int j = 0; j < mServer->mGame->mShapeVector.size(); j++)
		{                         //the client to send to's message        //the shape command it's about
			
			BuildDeltaMoveCommand(&mServer->mClientVector.at(i)->mMessage, mServer->mGame->mShapeVector.at(j));
		}
	}

	// Send messages to all clients
	mServer->SendPackets();

	// Store the sent command in history
	for (unsigned int i = 0; i < mServer->mGame->mShapeVector.size(); i++)
	{
		int num = (mServer->mGame->mShapeVector.at(i)->GetOutgoingSequence() - 1) & (COMMAND_HISTORY_SIZE-1);
		memcpy(&mServer->mGame->mShapeVector.at(i)->mFrame[num], &mServer->mGame->mShapeVector.at(i)->mCommand, sizeof(Command));
	}
}


//this is the whole shabang server exit not a player or shape exit
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

//this is just for clients right now, should i make another or hijack this function??
void Game::ReadDeltaMoveCommand(Message *mes, Client *client)
{
	int flags = 0;

	// Flags
	flags = mes->ReadByte();

	// Key
	if(flags & CMD_KEY)
	{
		client->mShape->mCommand.mKey = mes->ReadByte();

		//LogString("Client %d: read CMD_KEY (%d)", client->mShape->mIndex, client->mShape->mCommand.mKey);
	}

	// Read time to run command
	client->mShape->mCommand.mMilliseconds = mes->ReadByte();
	//LogString("msec:%d",client->mShape->mCommand.mMilliseconds);
	//let's set the shape's clientFrameTime right here.....
	client->mShape->mCommand.mClientFrametime = client->mShape->mCommand.mMilliseconds / 1000.0f;
}

void Game::BuildMoveCommand(Message *mes, Shape* shape)
{
	Command* command = &shape->mCommand;
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

//does this even care that a client is passed it? other than that it needs it access mShape? which
//i should already have?
void Game::BuildDeltaMoveCommand(Message *mes, Shape* shape)
{
	Command* command = &shape->mCommand;
	
	int flags = 0;

	int last = (shape->GetOutgoingSequence() - 1) & (COMMAND_HISTORY_SIZE-1);

	// Check what needs to be updated
	if(shape->mFrame[last].mKey != command->mKey)
	{
		flags |= CMD_KEY;
	}

	if(shape->mFrame[last].mOrigin.x != command->mOrigin.x ||
		shape->mFrame[last].mOrigin.z != command->mOrigin.z)
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
		mes->WriteByte(shape->mProcessedFrame & (COMMAND_HISTORY_SIZE-1));
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

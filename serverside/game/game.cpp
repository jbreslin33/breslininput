#include "game.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../server/server.h"
#include "../network/network.h"

#include "../client/client.h"
#include "../shape/shape.h"
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
	mFrameTime  = 0;

	mRunningShapeIndex = 1;
	mSpreadOutAIIndex = 1;
	
	for(int i = 0; i < 5; i++)
	{
		createAIShape();
	}
}

Game::~Game()
{
	StopLog();
	delete mServer;
}

void Game::createClientAvatar(Client* client)
{
	Vector3D* pos = new Vector3D();
	pos->x = 0;
	pos->y = 0;
	pos->z = 0;

	Vector3D* vel = new Vector3D();
	vel->x = 0;
	vel->y = 0;
	vel->z = 0;

	Vector3D* rot = new Vector3D();
	rot->x = 0;
	rot->z = 0;

	Shape* shape = new Shape(pos,vel,rot,mRoot,getOpenIndex()); 
	shape->mGame = this; //for now to give access to shapeVector for collision i guess
	mShapeVector.push_back(shape); //either way add this to shape vector

	if (client != NULL)
	{
		client->mShape = shape; 
		client->mShape->mClient = client; //set client it could be NULL meaning just a serverside shape
	}
}

unsigned int Game::getOpenIndex()
{
	bool proposedIndexOpen = false;
	for (unsigned int proposedIndex = 1; !proposedIndexOpen; proposedIndex++) //keep going till you get an index
	{
		bool someoneHasThisIndex = false;
		for (unsigned int i = 0; i < mShapeVector.size(); i++)
		{
			if (mShapeVector.at(i)->mIndex == proposedIndex)
			{
				someoneHasThisIndex = true;
			}
		}
		if (someoneHasThisIndex == false)
		{
			//proposedIndexOpen = true;
			return  proposedIndex;
		}

	}
	return 0;
}

void Game::createAIShape()
{
	Vector3D* pos = new Vector3D();
	pos->x = 0;
	pos->y = 0;
	pos->z = 300.0f * mSpreadOutAIIndex;;
	mSpreadOutAIIndex++;

	Vector3D* vel = new Vector3D();
	vel->x = 0;
	vel->y = 0;
	vel->z = 0;

	Vector3D* rot = new Vector3D();
	rot->x = 0;
	rot->z = 0;

	Shape* shape = new Shape(pos,vel,rot,mRoot,getOpenIndex()); 
	shape->mGame = this; //for now to give access to shapeVector for collision i guess
	mShapeVector.push_back(shape); //either way add this to shape vector

	mServer->sendAddAIShape(shape);
}

void Game::removeShape(Shape* shape)
{
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		if (mShapeVector.at(i) == shape)
		{
			mServer->sendRemoveShape(shape);
			mShapeVector.erase (mShapeVector.begin()+i);
		}
	}
}

void Game::frame(int msec)
{
	mRealTime += msec;

	mFrameTime += msec;

	// Read packets from clients
	mServer->readPackets();

	//just processtick for ai guys because their moves come from ai class/states
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		if (mShapeVector.at(i)->mClient == NULL) //your an ai guy
		{
			mShapeVector.at(i)->processTick();
		}
	}

	checkCollisions();

	// Wait full 32 ms before allowing to send
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

	sendCommand();
	mFrameTime = 0;
}

void Game::checkCollisions(void)
{
   for (unsigned int i = 0; i < mShapeVector.size(); i++)
   {
	  
	   for (unsigned int j = i+1; j < mShapeVector.size(); j++) 
	   {
		   
          float x1 = mShapeVector.at(i)->mSceneNode->getPosition().x;
	      float z1 = mShapeVector.at(i)->mSceneNode->getPosition().z;
		  float x2 = mShapeVector.at(j)->mSceneNode->getPosition().x;
	      float z2 = mShapeVector.at(j)->mSceneNode->getPosition().z;

		  float distSq = pow((x1-x2),2) + pow((z1-z2),2);

		  if(distSq < 10000.0)
		  {
			 mShapeVector.at(i)->mCommand.mOrigin = mShapeVector.at(i)->mCommand.mOriginOld;
			 mShapeVector.at(j)->mCommand.mOrigin = mShapeVector.at(j)->mCommand.mOriginOld;

             float x3 = mShapeVector.at(i)->mCommand.mOriginOld.x;
	         float z3 = mShapeVector.at(i)->mCommand.mOriginOld.z;
		     float x4 = mShapeVector.at(j)->mCommand.mOriginOld.x;
	         float z4 = mShapeVector.at(j)->mCommand.mOriginOld.z;

			
             mShapeVector.at(i)->mSceneNode->setPosition(x3,0.0,z3);
			 mShapeVector.at(j)->mSceneNode->setPosition(x4,0.0,z4);
		  }
	   }
   }
}

bool Game::checkScope(Client* client, Shape* shape)
{
	//let's check scop here...
	float x1 = client->mShape->mSceneNode->getPosition().x;  //clientshape
	float z1 = client->mShape->mSceneNode->getPosition().z;
	float x2 = shape->mSceneNode->getPosition().x;  //build shape
	float z2 = shape->mSceneNode->getPosition().z;

	float distSq = pow((x1-x2),2) + pow((z1-z2),2);
			
	if(distSq < 10000000.0)
	{
		return true;
	}
	else 
	{
		return false;
	}
}

//send to updates to all clients about all shapes
void Game::sendCommand(void)
{
	// Fill messages..for all clients
	for (unsigned int i = 0; i < mServer->mClientVector.size(); i++)
	{
		//standard initialize of mMessage for client in this case
		mServer->mClientVector.at(i)->mMessage.Init(mServer->mClientVector.at(i)->mMessage.outgoingData,
			sizeof(mServer->mClientVector.at(i)->mMessage.outgoingData));

		//start filling said mMessage that belongs to client
		mServer->mClientVector.at(i)->mMessage.WriteByte(USER_MES_FRAME);			// type
		mServer->mClientVector.at(i)->mMessage.WriteShort(mServer->mClientVector.at(i)->mOutgoingSequence);

		//this is where you need to actually loop thru the shapes not the clients but put write to client mMessage
		for (unsigned int j = 0; j < mShapeVector.size(); j++)
		{                         //the client to send to's message        //the shape command it's about
			if (checkScope(mServer->mClientVector.at(i),mShapeVector.at(i)))
			{
				buildDeltaMoveCommand(&mServer->mClientVector.at(i)->mMessage, mShapeVector.at(j));
			}
		}
	}

	// Send messages to all clients
	mServer->sendPackets();

	// Store the sent command in mLastCommand.
	for (unsigned int i = 0; i < mServer->mGame->mShapeVector.size(); i++)
	{
		memcpy(&mServer->mGame->mShapeVector.at(i)->mLastCommand, &mServer->mGame->mShapeVector.at(i)->mCommand, sizeof(Command));
		mServer->mGame->mShapeVector.at(i)->mCommand.mMillisecondsTotal = 0;
	}
}


//this is the whole shabang server exit not a player or shape exit
void Game::sendExitNotification(void)
{
	for (unsigned int i = 0; i < mServer->mClientVector.size(); i++)
	{

		mServer->mClientVector.at(i)->mMessage.Init(mServer->mClientVector.at(i)->mMessage.outgoingData,
			sizeof(mServer->mClientVector.at(i)->mMessage.outgoingData));

		mServer->mClientVector.at(i)->mMessage.WriteByte(USER_MES_SERVEREXIT);	// type
		mServer->mClientVector.at(i)->mMessage.WriteShort(mServer->mClientVector.at(i)->mOutgoingSequence);
	}

	mServer->sendPackets();
}

//this is just for clients right now, should i make another or hijack this function??
void Game::readDeltaMoveCommand(Message *mes, Client *client)
{
	int flags = 0;

	// Flags
	flags = mes->ReadByte();

	// Key
	if(flags & CMD_KEY)
	{
		client->mShape->mCommand.mKey = mes->ReadByte();
	}

	// Milliseconds
	if(flags & CMD_MILLISECONDS)
	{
		client->mShape->mCommand.mMilliseconds = mes->ReadByte();
	}

	//let's keep a tally called mMillisecondsTotal by adding up everytime we ReadDeltaMove...
	client->mShape->mCommand.mMillisecondsTotal += client->mShape->mCommand.mMilliseconds;

	//do i want to tally up the mKeys too???? especially if I'm not going to act on them as soon as i read them, atleast that is the plan.

	//let's set the shape's clientFrameTime right here.....
	client->mShape->mCommand.mClientFrametime = client->mShape->mCommand.mMilliseconds / 1000.0f;
}

//does this even care that a client is passed it? other than that it needs it access mShape? which
//i should already have?
void Game::buildDeltaMoveCommand(Message *mes, Shape* shape)
{
	Command* command = &shape->mCommand;
	
	int flags = 0;

/********** CHECK WHAT NEEDS TO BE UPDATED *****************/

	//Origin
	if(shape->mLastCommand.mOrigin.x != command->mOrigin.x)
	{
		flags |= CMD_ORIGIN_X;
	}
	if(shape->mLastCommand.mOrigin.y != command->mOrigin.y)
	{
		flags |= CMD_ORIGIN_Y;
	}
	if(shape->mLastCommand.mOrigin.z != command->mOrigin.z)
	{
		flags |= CMD_ORIGIN_Z;
	}

	//Rotation
	if(shape->mLastCommand.mRot.x != command->mRot.x)
	{
		flags |= CMD_ROTATION_X;
	}
	if(shape->mLastCommand.mRot.z != command->mRot.z)
	{
		flags |= CMD_ROTATION_Z;
	}
	
	//Milliseconds
	if(shape->mLastCommand.mMillisecondsTotal != command->mMillisecondsTotal)
	{
		flags |= CMD_MILLISECONDS;
	}

	/******ADD TO THE MESSAGE *****/

	//check scope first....
	//let's check within 10000
	

	mes->WriteByte(shape->mIndex);

	// Flags
	mes->WriteByte(flags);  

	//Origin
	if(flags & CMD_ORIGIN_X)
	{
		mes->WriteFloat(command->mOrigin.x);
	}
	if(flags & CMD_ORIGIN_Y)
	{
		mes->WriteFloat(command->mOrigin.y);
	}
	if(flags & CMD_ORIGIN_Z)
	{
		mes->WriteFloat(command->mOrigin.z);
	}


	//Rotation
	if(flags & CMD_ROTATION_X)
	{
		mes->WriteFloat(command->mRot.x);
	}
	if(flags & CMD_ROTATION_Z)
	{
		mes->WriteFloat(command->mRot.z);
	}

	//Milliseconds
	if(flags & CMD_MILLISECONDS)
	{
		mes->WriteByte(command->mMillisecondsTotal);
	}

	//shape->mCommand.mMillisecondsTotal = 0;
}

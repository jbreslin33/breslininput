#include "move.h"

//just using this for logstring need to fix that
#include "../tdreamsock/dreamSock.h"

//player,client,shape,animation combo
#include "../player/clientSidePlayer.h"
#include "../client/clientSideClient.h"
#include "../shape/ogreShape.h"
#include "../animation/ogreAnimation.h"

//move states
#include "moveStateMachine.h"
#include "moveStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8

Move::Move(ClientSidePlayer* player)
{
	mPlayer = player;

	mRunSpeed  = 200.0;
	mTurnSpeed = 250.0;
	mPosInterpLimitHigh = 8.0; //how far away from server till we try to catch up
	mPosInterpLimitLow  = 2.0; //how close to server till we are in sync
	mPosInterpFactor    = 10.0;

	//deltas
	mDeltaX        = 0.0; 
	mDeltaZ		   = 0.0;
	mDeltaPosition = 0.0;

	//move states
	mMoveStateMachine = new MoveStateMachine(this);    //setup the state machine
	mMoveStateMachine->setCurrentState      (Normal_Move::Instance());
	mMoveStateMachine->setPreviousState     (Normal_Move::Instance());
	mMoveStateMachine->setGlobalState       (NULL);
	//mMoveStateMachine->changeState        (Normal_Move::Instance());
}

Move::~Move()
{
}

void Move::processTick()
{
	mDeltaX = mPlayer->mClient->mServerFrame.mOrigin.x - mPlayer->mShape->getSceneNode()->getPosition().x;
    mDeltaZ = mPlayer->mClient->mServerFrame.mOrigin.z - mPlayer->mShape->getSceneNode()->getPosition().z;

	//distance we are off from server
	mDeltaPosition = sqrt(pow(mDeltaX, 2) + pow(mDeltaZ, 2));

	// if server has come to a stop
	if(mPlayer->mClient->mServerFrame.mVelocity.x == 0.0 && mPlayer->mClient->mServerFrame.mVelocity.z == 0.0)
	{
		mPlayer->mClient->mCommand.mStop = true;
	}
	else //server still moving
	{
		mPlayer->mClient->mCommand.mStop = false;
	}

	mMoveStateMachine->update();
	//processRotation();
}

void Move::interpolateTick(float renderTime)
{
	Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

	transVector.x = mPlayer->mClient->mCommand.mVelocity.x;
	transVector.z = mPlayer->mClient->mCommand.mVelocity.z;
	mPlayer->mShape->getSceneNode()->translate(transVector * renderTime * 1000, Ogre::Node::TS_WORLD);

	mPlayer->mShape->mOgreAnimation->updateAnimations(renderTime,mPlayer->mClient->mCommand.mStop);
}


void Move::calculateVelocity(Command* command, float frametime)
{
 	float multiplier = 200.0f;

 	command->mVelocity.x = 0.0f;
 	command->mVelocity.z = 0.0f;

 	if(command->mKey & KEY_UP)
	{
		command->mVelocity.z += -1;
	}

	if(command->mKey & KEY_DOWN)
	{
        command->mVelocity.z += 1;
	}

	if(command->mKey & KEY_LEFT)
	{
		command->mVelocity.x += -1;
	}

	if(command->mKey & KEY_RIGHT)
	{
		command->mVelocity.x += 1;
	}

	float length = sqrt(pow(command->mVelocity.x, 2) + pow(command->mVelocity.z, 2));
	if(length != 0.0)
	{
	   command->mVelocity.x = command->mVelocity.x/length * 0.2 * frametime * 1000;
	   command->mVelocity.z = command->mVelocity.z/length * 0.2 * frametime * 1000;
	}
}


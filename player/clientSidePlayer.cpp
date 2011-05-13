#include "clientSidePlayer.h"

//just using this for logstring need to fix that
#include "../tdreamsock/dreamSock.h"

//client,shape,animation combo
#include "../client/clientSideClient.h"
#include "../shape/ogreShape.h"
#include "../animation/ogreAnimation.h"

//states
#include "states/moveStateMachine.h"
#include "states/moveStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8

ClientSidePlayer::ClientSidePlayer(std::string name, ClientSideClient* client, OgreShape* shape) : Player(name)
{
	mClient = client;
	mShape  = shape;
	mServerRotSpeed = 0.0;
	mRunSpeed  = 200.0;
	mTurnSpeed = 250.0;
	mPosInterpLimitHigh = 8.0; //how far away from server till we try to catch up
	mPosInterpLimitLow  = 2.0; //how close to server till we are in sync
	mPosInterpFactor    = 10.0;
	mRotInterpLimitHigh = 6.0; //how far away from server till we try to catch up
	mRotInterpLimitLow  = 4.0; //how close to server till we are in sync
	mRotInterpIncrease  = 1.20; //rot factor used to catchup to server
    mRotInterpDecrease  = 0.80; //rot factor used to allow server to catchup to client

	//deltas
	mDeltaX        = 0.0; 
	mDeltaZ		   = 0.0;
	mDeltaPosition = 0.0;

	//states
	mMoveStateMachine = new MoveStateMachine(this);    //setup the state machine
	mMoveStateMachine->setCurrentState      (Normal_Move::Instance());
	mMoveStateMachine->setPreviousState     (Normal_Move::Instance());
	mMoveStateMachine->setGlobalState       (NULL);
	//mMoveStateMachine->changeState        (Normal_Move::Instance());

}

ClientSidePlayer::~ClientSidePlayer()
{
}

void ClientSidePlayer::processTick()
{
	mDeltaX = mClient->mServerFrame.mOrigin.x - mShape->getSceneNode()->getPosition().x;
    mDeltaZ = mClient->mServerFrame.mOrigin.z - mShape->getSceneNode()->getPosition().z;

	//distance we are off from server
	mDeltaPosition = sqrt(pow(mDeltaX, 2) + pow(mDeltaZ, 2));

	// if server has come to a stop
	if(mClient->mServerFrame.mVelocity.x == 0.0 && mClient->mServerFrame.mVelocity.z == 0.0)
	{
		mClient->mCommand.mStop = true;
	}
	else //server still moving
	{
		mClient->mCommand.mStop = false;
	}

	mMoveStateMachine->update();
	processRotation();
}

void ClientSidePlayer::interpolateTick(float renderTime)
{
	Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

	transVector.x = mClient->mCommand.mVelocity.x;
	transVector.z = mClient->mCommand.mVelocity.z;
	mShape->getSceneNode()->translate(transVector * renderTime * 1000, Ogre::Node::TS_WORLD);

	mShape->mOgreAnimation->updateAnimations(renderTime,mClient->mCommand.mStop);
	interpolateRotation(renderTime);
}


void ClientSidePlayer::processRotation()
{

	Ogre::Vector3 serverRotOld  = Ogre::Vector3::ZERO;
	Ogre::Vector3 serverRotNew  = Ogre::Vector3::ZERO;

	serverRotOld.x = mClient->mServerFrame.mRotOld.x;
	serverRotOld.z = mClient->mServerFrame.mRotOld.z;

	serverRotNew.x = mClient->mServerFrame.mRot.x;
	serverRotNew.z = mClient->mServerFrame.mRot.z;

	serverRotNew.normalise();
	serverRotOld.normalise();

	//calculate how far off we are from server
    Quaternion toServer = mShape->getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew,Vector3::UNIT_Y);

	// convert to degrees
	Real degreesToServer = toServer.getYaw().valueDegrees();

	// are we too far off
	if(abs(degreesToServer) > mRotInterpLimitHigh)
       mClient->mCommand.mCatchupRot = true;

	//calculate server rotation from last tick to new one
	Quaternion serverRot = mClient->mServerPlayer->mShape->getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew, Vector3::UNIT_Y);

    // convert to degrees
    mServerRotSpeed = serverRot.getYaw().valueDegrees();

	// yaw server guy to new rot
	mClient->mServerPlayer->mShape->getSceneNode()->yaw(Degree(mServerRotSpeed));

	//if we're rotating and need to catch up to server
	if(mServerRotSpeed != 0.0 && mClient->mCommand.mCatchupRot == true)
	{
       // if server rot counter-clockwise
	    if(mServerRotSpeed > 0.0)
          mClient->mCommand.mRotSpeed = mTurnSpeed;
	    else //clockwise
          mClient->mCommand.mRotSpeed = -mTurnSpeed;

		// if we are rotating counter-clockwise to catch up
		if(degreesToServer/mServerRotSpeed > 0.0)
           mClient->mCommand.mRotSpeed = mClient->mCommand.mRotSpeed * mRotInterpIncrease;
		// if we are rotating clockwise to catch up
		else
           mClient->mCommand.mRotSpeed = mClient->mCommand.mRotSpeed * mRotInterpDecrease;
	}
	// if server is not rotating but we still need to catchup
	else if(mServerRotSpeed == 0.0 && mClient->mCommand.mCatchupRot == true)
	{
	   //counter clockwise
       if (degreesToServer > 0.0)
		  mClient->mCommand.mRotSpeed = mTurnSpeed;
	   else //clockwise
          mClient->mCommand.mRotSpeed = -mTurnSpeed;
	}
	// if server is not rotating and we don't need to catch up
	else if (mServerRotSpeed == 0.0)
	{
       mClient->mCommand.mRotSpeed = 0.0;
	}
	// just rotating at same speed as server
	else
	{
       // if server rot counter-clockwise
	    if(mServerRotSpeed > 0.0)
          mClient->mCommand.mRotSpeed = mTurnSpeed;
	   else //clockwise
          mClient->mCommand.mRotSpeed = -mTurnSpeed;
	}
}

void ClientSidePlayer::interpolateRotation(float renderTime)
{
    float rotSpeed = mClient->mCommand.mRotSpeed * renderTime;
	mShape->getSceneNode()->yaw(Degree(rotSpeed));

	Ogre::Vector3 serverRotNew  = Ogre::Vector3::ZERO;

	serverRotNew.x = mClient->mServerFrame.mRot.x;
	serverRotNew.z = mClient->mServerFrame.mRot.z;

	serverRotNew.normalise();

	//calculate how far off we are from server
    Quaternion toServer = mShape->getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew,Vector3::UNIT_Y);

	// convert to degrees
	Real degreesToServer = toServer.getYaw().valueDegrees();

	// are we back in sync
	if(abs(degreesToServer) < mRotInterpLimitLow)
       mClient->mCommand.mCatchupRot = false;

	if (mServerRotSpeed == 0.0 && mClient->mCommand.mCatchupRot == false)
       mClient->mCommand.mRotSpeed = 0.0;

}


void ClientSidePlayer::calculateVelocity(Command* command, float frametime)
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


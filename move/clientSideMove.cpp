#include "clientSideMove.h"
#include "../tdreamsock/dreamSockLog.h"

#include <string>

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//move states
#include "moveStateMachine.h"
#include "moveStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16

ClientSideMove::ClientSideMove(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh)
 : ClientSideShape(name, position,mSceneMgr,mesh)
{
	//mPlayer = player;

	mRunSpeed     = 0.0;
	mRunSpeedMax  = 500.0;
	//mAccel        = 50.0
	
	mPosInterpLimitHigh = 8.0; //how far away from server till we try to catch up
	mPosInterpLimitLow  = 2.0; //how close to server till we are in sync
	mPosInterpFactor    = 4.0;

	//deltas
	mDeltaX        = 0.0; 
	mDeltaZ		   = 0.0;
	mDeltaY        = 0.0;
	mDeltaPosition = 0.0;

	//move states
	mMoveStateMachine = new MoveStateMachine(this);    //setup the state machine
	mMoveStateMachine->setCurrentState      (Normal_Move::Instance());
	mMoveStateMachine->setPreviousState     (Normal_Move::Instance());
	mMoveStateMachine->setGlobalState       (NULL);
	//mMoveStateMachine->changeState        (Normal_Move::Instance());
}

ClientSideMove::~ClientSideMove()
{
}

void ClientSideMove::processTick()
{
/*
	mDeltaX = mPlayer->mServerFrame.mOrigin.x - mPlayer->getSceneNode()->getPosition().x;
    mDeltaZ = mPlayer->mServerFrame.mOrigin.z - mPlayer->getSceneNode()->getPosition().z;
    mDeltaY = mPlayer->mServerFrame.mOrigin.y - mPlayer->getSceneNode()->getPosition().y;

        //distance we are off from server
        mDeltaPosition = sqrt(pow(mDeltaX, 2) + pow(mDeltaZ, 2) + pow(mDeltaY, 2));

        // if server has come to a stop
        if(mPlayer->mServerFrame.mVelocity.x == 0.0 && mPlayer->mServerFrame.mVelocity.z == 0.0
                && mPlayer->mServerFrame.mVelocity.y == 0.0)
        {
                mPlayer->mCommand.mStop = true;
        }
        else //server still moving
        {
                mPlayer->mCommand.mStop = false;
        }

        mMoveStateMachine->update();
*/
}

void ClientSideMove::interpolateTick(float renderTime)
{
/*
        Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

        transVector.x = mPlayer->mCommand.mVelocity.x;
        transVector.z = mPlayer->mCommand.mVelocity.z;
        transVector.y = mPlayer->mCommand.mVelocity.y;
        
        mPlayer->getSceneNode()->translate(transVector * renderTime * 1000, Ogre::Node::TS_WORLD);

        if(mPlayer->getSceneNode()->getPosition().y < 0.0)
		{
			mPlayer->getSceneNode()->setPosition(mPlayer->getSceneNode()->getPosition().x,
			0.0, mPlayer->getSceneNode()->getPosition().z);
		}
        float animSpeed = mRunSpeed * 1000/mRunSpeedMax;

		float yVelocity = mPlayer->mServerFrame.mVelocity.y;

      //  mPlayer->updateAnimations(yVelocity, renderTime, mPlayer->mCommand.mStop, animSpeed);
*/
}


void ClientSideMove::calculateVelocity(Command* command, float frametime)
{
/*
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
           command->mVelocity.x = command->mVelocity.x/length * mRunSpeed * frametime;
           command->mVelocity.z = command->mVelocity.z/length * mRunSpeed * frametime;
        }
*/
}


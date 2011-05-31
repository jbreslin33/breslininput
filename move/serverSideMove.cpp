#include "serverSideMove.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../client/client.h"

#define MAX_RUN_SPEED 200           // character running speed in units per second


#include <string>

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//move states
#include "serverSideMoveStateMachine.h"
#include "serverSideMoveStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16

ServerSideMove::ServerSideMove(std::string name, Vector3D* position, Ogre::Root* root)
:
	OgreShape(name,position,root),
	Move     (                  ) 
{
	


	//run acceleration
    mRunAccel    = 5.0;
    mRunDecel    = 5.0;

	//jumping
    mJumping  = false;
	mVerticalVelocity = 0.0;
    mJumpAccel   = 300.0;
    mGravity     = 900.0;

  	//move states
	mMoveStateMachine = new ServerSideMoveStateMachine(this);    //setup the state machine
	mMoveStateMachine->setCurrentState      (Normal_Move::Instance());
	mMoveStateMachine->setPreviousState     (Normal_Move::Instance());
	mMoveStateMachine->setGlobalState       (NULL);
}

ServerSideMove::~ServerSideMove()
{
}
void ServerSideMove::processTick()
{
	float clientFrametime;
    clientFrametime = mCommand.mMilliseconds / 1000.0f;;

    if (mCommand.mKey == 0)
	{
		if(mRunSpeed > 0.0)
		{
			mRunSpeed -= mRunDecel;
		}
        else
		{
           mRunSpeed = 0.0;
		}
    }
	else
	{
        if(mRunSpeed < MAX_RUN_SPEED)
		{
			mRunSpeed += mRunAccel;
		}
	}

    // move in current body direction (not the goal direction)
    mSceneNode->translate(0, 0, clientFrametime * mRunSpeed,Node::TS_LOCAL);

	/********************** JUMP **********************/
    if(mCommand.mKey & KEY_SPACE) 
    {
		if(!mJumping)
		{
			startJump();
	    }
    }

    if(mJumping)
    {
        mSceneNode->translate(0, clientFrametime * mVerticalVelocity, 0, Node::TS_LOCAL);
        mVerticalVelocity -= mGravity * clientFrametime;

        if(mSceneNode->getPosition().y < 0.0)
        {
            mSceneNode->setPosition(mSceneNode->getPosition().x, 0.0, mSceneNode->getPosition().z);
            mVerticalVelocity = 0.0;
            mJumping = false;
        }
	}
	/****************** END JUMP ************************/
}


void ServerSideMove::startJump()
{
	mJumping = true;
	mVerticalVelocity = mJumpAccel;
}

#include "serverSideJump.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../client/client.h"

#define MAX_RUN_SPEED 200           // character running speed in units per second

#include <string>

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//move states
#include "serverSideJumpStateMachine.h"
#include "serverSideJumpStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_SPACE				16

ServerSideJump::ServerSideJump(std::string name, Vector3D* position, Ogre::Root* root)
:
	OgreShape(name,position,root)
{
	//jumping
    mJumping  = false;
	mVerticalVelocity = 0.0;
    mJumpAccel   = 300.0;
    mGravity     = 900.0;

  	//move states
	mJumpStateMachine = new ServerSideJumpStateMachine(this);    //setup the state machine
	mJumpStateMachine->setCurrentState      (Normal_Jump::Instance());
	mJumpStateMachine->setPreviousState     (Normal_Jump::Instance());
	mJumpStateMachine->setGlobalState       (NULL);
}

ServerSideJump::~ServerSideJump()
{
}
void ServerSideJump::processTick()
{
	float clientFrametime;
    clientFrametime = mCommand.mMilliseconds / 1000.0f;;

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
}


void ServerSideJump::startJump()
{
	mJumping = true;
	mVerticalVelocity = mJumpAccel;
}

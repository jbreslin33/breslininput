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
	mJumpStateMachine->update();
}


void ServerSideJump::startJump()
{
	mVerticalVelocity = mJumpAccel;
}

#include "jump.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../client/client.h"

#define MAX_RUN_SPEED 15           // character running speed in units per second

#include <string>

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//move states
#include "jumpStateMachine.h"
#include "jumpStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_SPACE				16

Jump::Jump(Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::Root* root)
:
	OgreShape(position,velocity,rotation,root)
{
	//jumping
	mVerticalVelocity = 0.0;
    mJumpAccel   = 30.0;
    mGravity     = 90.0;

  	//move states
	mJumpStateMachine = new JumpStateMachine(this);    //setup the state machine
	mJumpStateMachine->setCurrentState      (Normal_Jump::Instance());
	mJumpStateMachine->setPreviousState     (Normal_Jump::Instance());
	mJumpStateMachine->setGlobalState       (NULL);
}

Jump::~Jump()
{
	delete mJumpStateMachine;
}
void Jump::processTick()
{
	mJumpStateMachine->update();
}


void Jump::startJump()
{
	mVerticalVelocity = mJumpAccel;
}

#include "rotation.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../shape/shape.h"

#include <string>

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//rotation states
#include "rotationStateMachine.h"
#include "rotationStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8

#define TURN_SPEED 250.0f      // character turning in degrees per second

Rotation::Rotation(std::string name, Vector3D* position,
									   Ogre::Root* root)
:
	OgreShape(name,position,root)
{
	//keys
    mKeyDirection = Vector3::ZERO;
	mGoalDirection = Vector3::ZERO;

	//rotation states
	mRotationStateMachine = new RotationStateMachine(this);    //setup the state machine
	mRotationStateMachine->setCurrentState      (Normal_Rotation::Instance());
	mRotationStateMachine->setPreviousState     (Normal_Rotation::Instance());
	mRotationStateMachine->setGlobalState       (NULL);
}

Rotation::~Rotation()
{
}

void Rotation::processTick()
{
	mRotationStateMachine->update();
}

void Rotation::setKeyDirection()
{
	mKeyDirection.x = 0;
    mKeyDirection.z = 0;
    mKeyDirection.y = 0;
                
    // keep track of the player's intended direction
    if(mCommand.mKey & KEY_UP) 
	{
		mKeyDirection.z += -1;
	}
    if(mCommand.mKey & KEY_LEFT) 
	{
		mKeyDirection.x += -1;
	}
                
    if(mCommand.mKey & KEY_DOWN) 
	{
        mKeyDirection.z += 1;
	}
    if(mCommand.mKey & KEY_RIGHT) 
	{
		mKeyDirection.x += 1;
	}
}
#include "rotation.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../shape/shape.h"

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

Rotation::Rotation()
:
	OgreShape()
{
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



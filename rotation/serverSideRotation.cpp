#include "serverSideRotation.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../shape/serverSideShape.h"

#include <string>

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//rotation states
#include "serverSideRotationStateMachine.h"
#include "serverSideRotationStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16

ServerSideRotation::ServerSideRotation(std::string name, Vector3D* position,
									   Ogre::Root* root)
:
	OgreShape(name,position,root),
	Rotation (                  )
{
	//rotation states
	mRotationStateMachine = new ServerSideRotationStateMachine(this);    //setup the state machine
	mRotationStateMachine->setCurrentState      (Normal_Rotation::Instance());
	mRotationStateMachine->setPreviousState     (Normal_Rotation::Instance());
	mRotationStateMachine->setGlobalState       (NULL);
}

ServerSideRotation::~ServerSideRotation()
{
}

void ServerSideRotation::processTick()
{
	
}

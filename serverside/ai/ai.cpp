#include "ai.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../client/client.h"

#define MAX_RUN_SPEED 200           // character running speed in units per second

#include <string>

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//ai states
#include "aiStateMachine.h"
#include "aiStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8

AI::AI(std::string name, Vector3D* position, Ogre::Root* root)
:
	OgreShape(name,position,root)
{
	//runSpeed
	mRunSpeed     = 0.0;

	//run acceleration
    mRunAccel    = 5.0;
    mRunDecel    = 5.0;

 	//ai states
	mAIStateMachine = new AIStateMachine(this);    //setup the state machine
	mAIStateMachine->setCurrentState      (Normal_AI::Instance());
	mAIStateMachine->setPreviousState     (Normal_AI::Instance());
	mAIStateMachine->setGlobalState       (NULL);
}

AI::~AI()
{
}
void AI::processTick()
{
	mAIStateMachine->update();
}



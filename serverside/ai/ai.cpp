#include "ai.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../../serverside/client/client.h"

#define MAX_RUN_SPEED 15           // character running speed in units per second

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

AI::AI()
:
	OgreShape()
{
 	//ai states
	mAIStateMachine = new AIStateMachine(this);    //setup the state machine
	mAIStateMachine->setCurrentState      (Random_AI::Instance());
	mAIStateMachine->setPreviousState     (Random_AI::Instance());
	mAIStateMachine->setGlobalState       (NULL);
}

AI::~AI()
{
}
void AI::processTick()
{
	mAIStateMachine->update();
}



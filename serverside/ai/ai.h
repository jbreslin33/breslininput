#ifndef SERVERSIDEAI_H
#define SERVERSIDEAI_H

//parents
#include "../../serverside/shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class AIStateMachine;

class AI : public virtual OgreShape
{

public:

AI();
~AI();

//ticks
void processTick    ();

AIStateMachine* mAIStateMachine;

};

#endif


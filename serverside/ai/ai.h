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

AI(Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::Root* root,unsigned int index);
~AI();

//ticks
void processTick    ();

AIStateMachine* mAIStateMachine;

};

#endif


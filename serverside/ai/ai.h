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

AI(std::string name, Vector3D* position, Ogre::Root* root);
~AI();

//ticks
void processTick    ();

AIStateMachine* mAIStateMachine;

//run speed
float mRunSpeed;

//run acceleration
float mRunDecel;
float mRunAccel;

};

#endif


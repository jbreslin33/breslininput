#ifndef SERVERSIDEMOVE_H
#define SERVERSIDEMOVE_H

//parents
#include "../../serverside/shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class MoveStateMachine;

class Move : public virtual OgreShape
{

public:

Move();
~Move();

//ticks
void processTick    ();

MoveStateMachine* mMoveStateMachine;

//run speed
float mRunSpeed;

//run acceleration
float mRunDecel;
float mRunAccel;

};

#endif


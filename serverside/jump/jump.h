#ifndef JUMP_H
#define JUMP_H

//parents
#include "../../serverside/shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class JumpStateMachine;

class Jump : public virtual OgreShape
{

public:

Jump();
~Jump();

//ticks
void processTick    ();
void startJump();

JumpStateMachine* mJumpStateMachine;

//jumping
float mVerticalVelocity;
float mJumpAccel;
float mGravity;
};

#endif


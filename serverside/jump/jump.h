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

Jump(Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::Root* root,unsigned int index);
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


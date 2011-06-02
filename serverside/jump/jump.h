#ifndef JUMP_H
#define JUMP_H

//parents
#include "../../serverside/shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class ServerSideJumpStateMachine;

class ServerSideJump : public virtual OgreShape
{

public:

ServerSideJump(std::string name, Vector3D* position, Ogre::Root* root);
~ServerSideJump();

//ticks
void processTick    ();
void startJump();

ServerSideJumpStateMachine* mJumpStateMachine;

//jumping
float mVerticalVelocity;
float mJumpAccel;
float mGravity;
};

#endif


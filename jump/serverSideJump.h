#ifndef SERVERSIDEJUMP_H
#define SERVERSIDEJUMP_H

//parents
#include "../shape/ogreShape.h"

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
bool  mJumping;
float mVerticalVelocity;
float mJumpAccel;
float mGravity;
};

#endif


#ifndef SERVERSIDEMOVE_H
#define SERVERSIDEMOVE_H

//parents
#include "move.h"
#include "../shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class ServerSideMoveStateMachine;
class Client;

class ServerSideMove : public virtual OgreShape, public Move 
{

public:

ServerSideMove(std::string name, Vector3D* position, Ogre::Root* root);
~ServerSideMove();

//ticks
void processTick    ();


void startJump();

ServerSideMoveStateMachine* mMoveStateMachine;



//run acceleration
float mRunDecel;
float mRunAccel;

//jumping
bool  mJumping;
float mVerticalVelocity;
float mJumpAccel;
float mGravity;
};

#endif


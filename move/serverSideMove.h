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

class ServerSideMove : public virtual OgreShape, public Move 
{

public:

ServerSideMove(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~ServerSideMove();

//ticks
void processTick    ();
void calculateVelocity(Command* command, float frametime);

ServerSideMoveStateMachine* mMoveStateMachine;
};

#endif


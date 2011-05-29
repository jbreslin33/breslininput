#ifndef CLIENTSIDEMOVE_H
#define CLIENTSIDEMOVE_H

//parents
#include "move.h"
#include "../shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;

class ClientSideMove : public virtual OgreShape, public Move 
{

public:

ClientSideMove(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~ClientSideMove();

//ticks
void processTick    ();
void interpolateTick    (float renderTime);
void calculateVelocity(Command* command, float frametime);

};

#endif


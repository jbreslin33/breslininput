#ifndef SHAPE_H
#define SHAPE_H

//parents
#include "../../move/move.h"
#include "../../rotation/rotation.h"
#include "../../animation/ogreAnimation.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

#include <string>

class Shape : public ClientSideMove , public ClientSideRotation, public OgreAnimation
{

public:
Shape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~Shape();

Shape* mServerShape;

//ticks
void processTick();
void interpolateTick(float renderTime);

//stats
Vector3D* mPosition;

//id
int			mIndex;
std::string mName;

Command mFrame[COMMAND_HISTORY_SIZE];
Command	mServerFrame;					// the latest frame from server
Command	mCommand;						// current frame's commands


};

#endif


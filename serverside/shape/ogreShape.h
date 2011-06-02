#ifndef OGRESHAPE_H
#define OGRESHAPE_H

//command
#include "../../command/command.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//utility
#include <string.h>

//math
#include "../../math/vector3D.h"

#define COMMAND_HISTORY_SIZE		64

class OgreShape 
{

public:
OgreShape();
OgreShape(std::string name, Vector3D* position, Ogre::Root* root);
~OgreShape();

//ogre scene stuff
SceneNode*          mSceneNode;

//stats
Vector3D* mPosition;

int			mIndex;
std::string mName;

Command mFrame[COMMAND_HISTORY_SIZE];
Command	mServerFrame;					// the latest frame from server
Command	mCommand;						// current frame's commands

long mProcessedFrame;
};

#endif


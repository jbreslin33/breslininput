#ifndef OGRESHAPE_H
#define OGRESHAPE_H

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//command
#include "../../command/command.h"

//utility
#include <string.h>

//math
#include "../../math/vector3D.h"

#define COMMAND_HISTORY_SIZE		64

class ObjectTitle;

class OgreShape
{

public:
OgreShape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
OgreShape();
~OgreShape();

SceneNode* getSceneNode() { return mSceneNode; }

//ogre scene stuff
std::string         mMeshName;
Entity*             mEntity;
Ogre::SceneManager* mSceneManager;
SceneNode*          mSceneNode;

//billboard
ObjectTitle* mObjectTitle;
std::string  mObjectTitleString;

//stats
Vector3D* mPosition;

//id
int			mIndex;
std::string mName;

Command mFrame[COMMAND_HISTORY_SIZE];
Command	mServerFrame;					// the latest frame from server
Command	mCommand;						// current frame's commands

long mProcessedFrame;
 

};

#endif


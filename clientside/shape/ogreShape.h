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
class Game;
class Shape;
class OgreShape
{

public:
OgreShape(int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::SceneManager* mSceneMgr, std::string mesh);
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

//run speed
float mRunSpeed;

float mRunSpeedMax;

//id
int			mIndex;
std::string  mName;

Command	mServerFrame;					// the latest frame from server
Command mCommandToRunOnShape;

float mRenderTime;



Game* mGame;
Shape* mGhost;


};

#endif


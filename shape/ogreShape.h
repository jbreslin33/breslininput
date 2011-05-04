#ifndef OGRESHAPE_H
#define OGRESHAPE_H

#include "shape.h"

#include "Ogre.h"
#include "OIS.h"

using namespace Ogre;

#include <string>

class OgreShape : Shape
{

public:

OgreShape(std::string shapeName, Vector3D* position, Ogre::SceneManager* sceneMgr, std::string meshName);
~OgreShape();

//stats
std::string mShapeName;


SceneNode* getSceneNode() { return mSceneNode; }

//stats
std::string mMeshName;

Entity* mEntity;

AnimationState      *mAnimationState;
Ogre::SceneManager* mSceneManager;
SceneNode*          mSceneNode;
Vector3D*           mPosition;


};

#endif


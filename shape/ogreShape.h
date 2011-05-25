#ifndef OGRESHAPE_H
#define OGRESHAPE_H

#include "shape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class OgreAnimation;

class OgreShape : public Shape
{

public:
OgreShape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
OgreShape(std::string name, Vector3D* position, Ogre::Root* root);
~OgreShape();

SceneNode* getSceneNode() { return mSceneNode; }

//ogre scene stuff
std::string         mMeshName;
Entity*             mEntity;
Ogre::SceneManager* mSceneManager;
SceneNode*          mSceneNode;

//animation
OgreAnimation* mOgreAnimation;

};

#endif


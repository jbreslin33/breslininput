#ifndef CLIENTSIDESHAPE_H
#define CLIENTSIDESHAPE_H

#include "shape.h"

#include "Ogre.h"
#include "OIS.h"

using namespace Ogre;

class ClientSideShape : public Shape
{

public:
ClientSideShape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~ClientSideShape();

virtual void setupModel();

SceneNode* getSceneNode() { return mSceneNode; }

//stats
std::string mMeshName;

Entity* mEntity;

AnimationState *mAnimationState;
Ogre::SceneManager* mSceneManager;
SceneNode*          mSceneNode;

};

#endif


#ifndef CLIENTSIDESHAPE_H
#define CLIENTSIDESHAPE_H

#include "shape.h"

#include "Ogre.h"
#include "OIS.h"

using namespace Ogre;

class ClientSideShape : public Shape
{

public:
ClientSideShape(Ogre::SceneManager* mSceneMgr, std::string name, int xPos, int yPos, int zPos, std::string mesh);
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


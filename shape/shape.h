#ifndef SHAPE_H
#define SHAPE_H

#include "Ogre.h"

using namespace Ogre;

class Shape
{

public:

Shape(Ogre::SceneManager* mSceneMgr, std::string name, int xPos, int yPos, int zPos,std::string mesh); 
~Shape();

SceneNode* getSceneNode() { return mSceneNode; }


virtual void setupModel();
void cleanupContent();

//objects
Ogre::SceneManager* mSceneManager;
SceneNode*          mSceneNode;
std::string mMeshName;

//stats
std::string mShapeName;
int xPos;
int yPos;
int zPos;

};

#endif


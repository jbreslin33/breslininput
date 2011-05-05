#include "ogreShape.h"
#include "../tdreamsock/dreamSock.h"

#include "../math/vector3D.h"


OgreShape::OgreShape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh) : Shape(name,position)
{
	//let's set our member variables to those passed in for use...
    mSceneManager = mSceneMgr;
	mMeshName     = mesh;
    mSceneNode    = mSceneManager->getRootSceneNode()->createChildSceneNode();


	// put character in starting spawn spot
    mSceneNode->translate(position->x,position->y,position->z, Node::TS_LOCAL);

	// create entity and attach mesh to it
    mEntity = mSceneManager->createEntity(mShapeName, mMeshName);
    mSceneNode->attachObject(mEntity);
}

OgreShape::~OgreShape()
{
}


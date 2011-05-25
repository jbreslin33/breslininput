#include "ogreShape.h"

#include "../animation/ogreAnimation.h"
#include "../math/vector3D.h"

#define CHAR_HEIGHT 5          // height of character's center of mass above ground

//constructor for client side ogre shape
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

	//create an animation instance and pass our entity in
	mOgreAnimation = new OgreAnimation(mEntity);
}
//this is the contructor for server side ogre shape
OgreShape::OgreShape(std::string name, Vector3D* position, Ogre::Root* root) : Shape(name,position)
{
	
	mSceneManager = root->createSceneManager(Ogre::ST_GENERIC);
	
	// create main model
	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y * CHAR_HEIGHT);
}

OgreShape::~OgreShape()
{
}


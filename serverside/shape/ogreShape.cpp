#include "ogreShape.h"

//math
#include "../../math/vector3D.h"

OgreShape::OgreShape()
{
}

//this is the contructor for server side ogre shape
OgreShape::OgreShape(Vector3D* position, Ogre::Root* root)
{
	Ogre::SceneManager* mSceneManager = root->createSceneManager(Ogre::ST_GENERIC);

	// create main model

	Vector3 spawnPoint;
	spawnPoint.x = position->x;
	spawnPoint.y = position->y;
	spawnPoint.z = position->z;

	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(spawnPoint);

 	mPosition = position;
	mIndex  = 0;
	mProcessedFrame = 0;

	outgoingSequence		= 1;

	//keys
    mKeyDirection = Vector3::ZERO;
	mGoalDirection = Vector3::ZERO;

	mGame = NULL;
}

OgreShape::~OgreShape()
{
	//delete mSceneManager;
	delete mSceneNode;
}


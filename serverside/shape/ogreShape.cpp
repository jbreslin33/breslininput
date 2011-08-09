#include "ogreShape.h"
#include "../tdreamsock/dreamSockLog.h"

//math
#include "../../math/vector3D.h"

OgreShape::OgreShape()
{
}

//this is the contructor for server side ogre shape
OgreShape::OgreShape(Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::Root* root,
					 unsigned int index)
{
	Ogre::SceneManager* mSceneManager = root->createSceneManager(Ogre::ST_GENERIC);

	// set command origins
	mCommand.mOrigin.x = position->x;
	mCommand.mOrigin.y = position->y;
	mCommand.mOrigin.z = position->z;

	//convert to ogre format
	Vector3 spawnPoint;
	spawnPoint.x = mCommand.mOrigin.x;
	spawnPoint.y = mCommand.mOrigin.y;
	spawnPoint.z = mCommand.mOrigin.z;

	//create node to represent shape on server and pass in spawnPoint
	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(spawnPoint);

 	mPosition = position;
	mIndex  = index;

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


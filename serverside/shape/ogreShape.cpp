#include "ogreShape.h"

//math
#include "../../math/vector3D.h"

OgreShape::OgreShape()
{
}

//this is the contructor for server side ogre shape
OgreShape::OgreShape(std::string name, Vector3D* position, Ogre::Root* root)
{
	Ogre::SceneManager* mSceneManager = root->createSceneManager(Ogre::ST_GENERIC);

	// create main model
	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(Vector3::ZERO);

 	mPosition = position;
	mIndex  = 0;
   	mName = name;
	mProcessedFrame = 0;

	outgoingSequence		= 1;
}

OgreShape::~OgreShape()
{
	//delete mSceneManager;
	delete mSceneNode;
}


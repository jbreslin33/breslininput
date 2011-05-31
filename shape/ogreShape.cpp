#include "ogreShape.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../math/vector3D.h"

//billboard
#include "../billboard/movableTextOverlay.h"

OgreShape::OgreShape()
{

}
//constructor for client side ogre shape
OgreShape::OgreShape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh)
:
	Shape(name,position)
{
	LogString("OgreShape Constructor");
	//let's set our member variables to those passed in for use...
    mSceneManager = mSceneMgr;
	mMeshName     = mesh;
	mName         = name;
    mSceneNode    = mSceneManager->getRootSceneNode()->createChildSceneNode();

	// put character in starting spawn spot
    mSceneNode->translate(position->x,position->y,position->z, Node::TS_LOCAL);

	// create entity and attach mesh to it
    mEntity = mSceneManager->createEntity(mName, mMeshName);
    mSceneNode->attachObject(mEntity);

//billboard
		// create the attributes used by MovableTextOverlay
	MovableTextOverlayAttributes *attrs = new MovableTextOverlayAttributes("Attrs1",mSceneMgr->getCamera("PlayerCam"),"BlueHighway",16,ColourValue::White,"RedTransparent");
	//mSceneMgr->getCamera("PlayerCam");
//	mText = new ObjectTextDisplay(mEntity, mSceneMgr->getCamera("PlayerCam"));
  //  mText->enable(true);
  //  mText->setText("Ogre Head");
//end billboard

}

//this is the contructor for server side ogre shape
OgreShape::OgreShape(std::string name, Vector3D* position, Ogre::Root* root)
:
	Shape(name,position)
{
	mText = NULL;
	
	mSceneManager = root->createSceneManager(Ogre::ST_GENERIC);
	
	// create main model
	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(Vector3::ZERO);
}

OgreShape::~OgreShape()
{
}


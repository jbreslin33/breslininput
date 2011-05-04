#include "clientSideShape.h"
#include "../tdreamsock/dreamSock.h"

#define RUN_SPEED 300

ClientSideShape::ClientSideShape(Ogre::SceneManager* sceneMgr, std::string shapeName,int x, int y, int z,std::string meshName) : Shape(shapeName,x,y,z)
{
	mSceneManager = sceneMgr;
   	mMeshName = meshName;
	mEntity = NULL;
	setupModel();
}

ClientSideShape::~ClientSideShape()
{
}

void ClientSideShape::setupModel()
{
    // create scene nodes for the models at regular angular intervals
    mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
                
    // put character in starting spawn spot
    mSceneNode->translate(x,y,z, Node::TS_LOCAL);


	// create entity and attach mesh to it
	mEntity = mSceneManager->createEntity(mShapeName, mMeshName);
	mSceneNode->attachObject(mEntity);
}


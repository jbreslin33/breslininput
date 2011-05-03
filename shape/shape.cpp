#include "shape.h"
#include "../tdreamsock/dreamSock.h"

Shape::Shape(Ogre::SceneManager* sceneMgr, std::string shapeName, int x, int y, int z,std::string meshName)
{
	mSceneManager = sceneMgr;
   	mShapeName = shapeName;
	mMeshName = meshName;

	//starting position
   	xPos = x;
   	yPos = y;
   	zPos = z;
}

Shape::~Shape()
{
}

void Shape::setupModel()
{
    // create scene nodes for the models at regular angular intervals
    mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
                
    // put character in starting spawn spot
    mSceneNode->translate(xPos,yPos,zPos, Node::TS_LOCAL);

	// create entity and attach mesh to it
	mEntity = mSceneManager->createEntity(mShapeName, mMeshName);
	mSceneNode->attachObject(mEntity);
}

void Shape::cleanupContent()
{
	mSceneNode      = NULL;
}










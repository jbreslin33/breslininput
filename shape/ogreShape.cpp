#include "ogreShape.h"
#include "../tdreamsock/dreamSock.h"
#include "../math/Vector3D.h"

OgreShape::OgreShape(std::string shapeName, Vector3D* position, Ogre::SceneManager* sceneMgr, std::string meshName) : Shape(shapeName,position)
{
   	mShapeName = shapeName;
	mPosition = position;
	mSceneManager = sceneMgr;
   	mMeshName = meshName;

    // create scene nodes for the models at regular angular intervals
    mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
                
    // put character in starting spawn spot
    mSceneNode->translate(mPosition->x,mPosition->y,mPosition->z, Node::TS_LOCAL);

	// create entity and attach mesh to it
	mEntity = mSceneManager->createEntity(mShapeName, mMeshName);
	mSceneNode->attachObject(mEntity);

}

OgreShape::~OgreShape()
{
}












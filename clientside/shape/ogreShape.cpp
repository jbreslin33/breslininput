#include "ogreShape.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../math/vector3D.h"

//billboard
#include "../../billboard/objectTitle.h"

OgreShape::OgreShape()
{
}
//constructor for client side ogre shape
OgreShape::OgreShape(Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh)
{
	static int number_of_times = 0;

 	mPosition = position;
	mIndex  = 0;
	mProcessedFrame = 0;
	mRenderTime = 0.0;

	//let's set our member variables to those passed in for use...
    mSceneManager = mSceneMgr;
	mMeshName     = mesh;
	mName         = StringConverter::toString(number_of_times);
    mSceneNode    = mSceneManager->getRootSceneNode()->createChildSceneNode();

	// put character in starting spawn spot
    mSceneNode->translate(position->x,position->y,position->z, Node::TS_LOCAL);

	mEntity = mSceneManager->createEntity(mName,
		mMeshName);
    mSceneNode->attachObject(mEntity);


	//billboard
	const Ogre::String& titlename = "tn" + StringConverter::toString(number_of_times);
	const Ogre::String& title = "ti" + StringConverter::toString(number_of_times);
	const Ogre::String& fontName = "SdkTrays/Caption";
	const Ogre::ColourValue& color = Ogre::ColourValue::White;
	mObjectTitle = new ObjectTitle
	(titlename, mEntity, mSceneMgr->getCamera("PlayerCam"), title,
    fontName, color);
	//end billboard

	number_of_times++;
}

OgreShape::~OgreShape()
{
	delete mObjectTitle;
	delete mEntity;
	delete mSceneNode;
}


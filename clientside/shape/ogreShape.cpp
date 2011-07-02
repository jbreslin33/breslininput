#include "ogreShape.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../math/vector3D.h"

//billboard
#include "../../billboard/objectTitle.h"

OgreShape::OgreShape()
{
}
//constructor for client side ogre shape
OgreShape::OgreShape(int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::SceneManager* mSceneMgr, std::string mesh)
{
	static int number_of_times = 0;

 	mPosition = position;
	mIndex  = ind;
	mRenderTime = 0.0;

	//let's set our member variables to those passed in for use...
    mSceneManager = mSceneMgr;
	mMeshName     = mesh;
	mName         = StringConverter::toString(number_of_times);
    mSceneNode    = mSceneManager->getRootSceneNode()->createChildSceneNode();

	// put character in starting spawn spot
    //mSceneNode->translate(position->x,position->z,position->y, Node::TS_LOCAL);

	LogString("index:%d",mIndex);
	LogString("x:%f",position->x);
	LogString("y:%f",position->y);
	LogString("z:%f",position->z);
	LogString("_______________");

	//let's set the serverframe...for inititialize purposeses
	mServerFrame.mOrigin.x = position->x;
	mServerFrame.mOrigin.y = position->y;
	mServerFrame.mOrigin.z = position->z;

	mServerFrame.mRot.x = rotation->x;
	mServerFrame.mRot.z = rotation->z;

	mSceneNode->setPosition(position->x,position->z,position->y);	

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

	mGame = NULL;
	mGhost = NULL;
}

OgreShape::~OgreShape()
{
	delete mObjectTitle;
	delete mEntity;
	delete mSceneNode;
}


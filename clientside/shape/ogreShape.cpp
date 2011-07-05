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

	//let's set the serverframe...for inititialize purposeses
	mServerFrame.mOrigin.x = position->x;
	mServerFrame.mOrigin.y = position->y;
	mServerFrame.mOrigin.z = position->z;

	mServerFrame.mOriginOld.x = position->x;
	mServerFrame.mOriginOld.y = position->y;
	mServerFrame.mOriginOld.z = position->z;

	mServerFrame.mVelocity.x = position->x;
	mServerFrame.mVelocity.y = position->y;
	mServerFrame.mVelocity.z = position->z;

	mServerFrame.mRot.x = rotation->x;
	mServerFrame.mRot.z = rotation->z;

	mServerFrame.mMilliseconds = 0;
	mServerFrame.mMillisecondsTotal = 0;

	mServerFrame.mKey = 0;
	mServerFrame.mRotSpeed = 0;

	//let's set the serverframe...for inititialize purposeses
	mCommandToRunOnShape.mOrigin.x = position->x;
	mCommandToRunOnShape.mOrigin.y = position->y;
	mCommandToRunOnShape.mOrigin.z = position->z;

	mCommandToRunOnShape.mOriginOld.x = position->x;
	mCommandToRunOnShape.mOriginOld.y = position->y;
	mCommandToRunOnShape.mOriginOld.z = position->z;

	mCommandToRunOnShape.mVelocity.x = position->x;
	mCommandToRunOnShape.mVelocity.y = position->y;
	mCommandToRunOnShape.mVelocity.z = position->z;

	mCommandToRunOnShape.mRot.x = rotation->x;
	mCommandToRunOnShape.mRot.z = rotation->z;

	mCommandToRunOnShape.mMilliseconds = 0;
	mCommandToRunOnShape.mMillisecondsTotal = 0;

	mCommandToRunOnShape.mKey = 0;
	mCommandToRunOnShape.mRotSpeed = 0;

	mSceneNode->setPosition(position->x,position->y,position->z);	

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


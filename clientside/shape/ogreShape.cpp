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
	//Ogre::SceneManager* mSceneManager = game->getRoot();

	static int number_of_times = 0;

	//let's set our member variables to those passed in for use...
    //mSceneManager = mSceneMgr;
	mMeshName     = mesh;
	mName         = StringConverter::toString(number_of_times);
    mSceneNode    = mGame->mSceneMgr->getRootSceneNode()->createChildSceneNode();

	mSceneNode->setPosition(position->x,position->y,position->z);	
	
	mEntity = mGame->mSceneMgr->createEntity(mName, mMeshName);

    mSceneNode->attachObject(mEntity);

	//billboard
	const Ogre::String& titlename = "tn" + StringConverter::toString(number_of_times);
	const Ogre::String& title = "ti" + StringConverter::toString(number_of_times);
	const Ogre::String& fontName = "SdkTrays/Caption";
	const Ogre::ColourValue& color = Ogre::ColourValue::White;
	mObjectTitle = new ObjectTitle
	(titlename, mEntity, mSceneMgr->getCamera("PlayerCam"), title,
    fontName, color);

	number_of_times++;
	//end billboard
}

OgreShape::~OgreShape()
{
	delete mObjectTitle;
	delete mEntity;
	delete mSceneNode;
}

void OgreShape::yaw(float amountToYaw, bool converToDegree)
{
	if (convertToDegree)
	{
		getSceneNode()->yaw(Degree(rotation->mServerRotSpeed), true);
		//rotation->mGhost->yaw(rotation->mServerRotSpeed,true);	
	}
}
float OgreShape::getDegreesToSomething(Vector3D vectorOfSomething)
{
    //calculate how far off we are from server
    Quaternion toSomething = getSceneNode()->getOrientation().zAxis().getRotationTo(vectorOfSomething,Vector3::UNIT_Y);


    // convert to degrees
    Real degreesToServer = toServer.getYaw().valueDegrees();
	return degreesToserver;
}

//1 world, 2 local
void OgreShape::translate(Vector3D translateVector, int perspective)
{
	if (perspective == 1)
	{
		move->getSceneNode()->translate(transVector * move->mRenderTime * 1000, Ogre::Node::TS_WORLD);
	}
	if (perspective == 2)
	{
		move->getSceneNode()->translate(transVector * move->mRenderTime * 1000, Ogre::Node::TS_LOCAL);
	}
}

void OgreShape::setPosition(Vector3D position)
{
	getSceneNode()->setPosition(position);
}

void OgreShape::setPosition(float x, float y, float z)
{
	getSceneNode()->setPosition(x,y,z);
}

Vector3D OgreShape::getPosition()
{
	Vector3D position;
	position.x = getSceneNode()->getPosition.x;
	position.y = getSceneNode()->getPosition.y;
	position.z = getSceneNode()->getPosition.z;
	return position;
}
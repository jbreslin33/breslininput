#include "ogreDynamicShape.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../game/game.h"

#include "../../billboard/objectTitle.h"

#include <string.h>

OgreDynamicShape::OgreDynamicShape(Game* game, int ind, Vector3D* position, Vector3D* velocity,
						   Vector3D* rotation, std::string mesh)
:
	DynamicShape         (game,ind,position,velocity,rotation)
{
	//Ogre::SceneManager* mSceneManager = game->getRoot();

	static int number_of_times = 0;
	
	//let's set our member variables to those passed in for use...
    //mSceneManager = mSceneMgr;
	mMeshName     = mesh;
	mName         = StringConverter::toString(number_of_times);
    mSceneNode    = mGame->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	LogString("create mSceneNode for OgreDynamicShape");
	mSceneNode->setPosition(position->x,position->y,position->z);	
	
	mEntity = mGame->getSceneManager()->createEntity(mName, mMeshName);

    mSceneNode->attachObject(mEntity);

	//billboard
	const Ogre::String& titlename = "tn" + StringConverter::toString(number_of_times);
	const Ogre::String& title = "ti" + StringConverter::toString(number_of_times);
	const Ogre::String& fontName = "SdkTrays/Caption";
	const Ogre::ColourValue& color = Ogre::ColourValue::White;
	mObjectTitle = new ObjectTitle
	(titlename, mEntity, mGame->getSceneManager()->getCamera("PlayerCam"), title,
    fontName, color);

	number_of_times++;
	//end billboard
}

OgreDynamicShape::~OgreDynamicShape()
{
	delete mObjectTitle;
	delete mEntity;
	delete mSceneNode;
}

void OgreDynamicShape::scale(Vector3D scaleVector)
{
	getSceneNode()->scale(scaleVector.x, scaleVector.y, scaleVector.z);
}

void OgreDynamicShape::checkExtents(Vector3D min, int i)
{

	Ogre::Vector3 max;
	max = Ogre::Vector3::UNIT_SCALE;

	assert( (min.x <= max.x && min.y <= max.y && min.z <= max.z) &&
                "Got ya Bitch!" );

			//mExtent = EXTENT_FINITE;
			//mMinimum = min;
			//mMaximum = max;
}
void OgreDynamicShape::yaw(float amountToYaw, bool convertToDegree)
{
	//LogString("before");
	if (convertToDegree)
	{
		getSceneNode()->yaw(Degree(amountToYaw));
		//rotation->mGhost->yaw(rotation->mServerRotSpeed,true);	
	}
	//LogString("after");
}
float OgreDynamicShape::getDegreesToSomething(Vector3D vectorOfSomething,int i)
{
		
	checkExtents(vectorOfSomething,1);
	//LogString("b");
    //calculate how far off we are from server

    Quaternion toSomething = getSceneNode()->getOrientation().zAxis().getRotationTo(converToVector3(vectorOfSomething),Vector3::UNIT_Y);


    // convert to degrees
    Real degreesToServer = toSomething.getYaw().valueDegrees();
	//LogString("a");
	return degreesToServer;
}

//1 world, 2 local
void OgreDynamicShape::translate(Vector3D translateVector, int perspective)
{
	//checkExtents(translateVector);
	if (perspective == 1)
	{
		getSceneNode()->translate(converToVector3(translateVector), Ogre::Node::TS_WORLD);
	}
	if (perspective == 2)
	{
		getSceneNode()->translate(converToVector3(translateVector), Ogre::Node::TS_LOCAL);
	}
}

void OgreDynamicShape::setPosition(Vector3D position)
{
	//checkExtents(position);
	getSceneNode()->setPosition(converToVector3(position));
}

void OgreDynamicShape::setPosition(float x, float y, float z)
{
	getSceneNode()->setPosition(x,y,z);
}

Vector3D OgreDynamicShape::getPosition()
{
	Vector3D position;
	position.x = getSceneNode()->getPosition().x;
	position.y = getSceneNode()->getPosition().y;
	position.z = getSceneNode()->getPosition().z;
	return position;
}

Ogre::Vector3 OgreDynamicShape::converToVector3(Vector3D vector3d)
{
		//checkExtents(vector3d);
	Ogre::Vector3 vec3;
	vec3.x = vector3d.x;
	vec3.y = vector3d.y;
	vec3.z = vector3d.z;
	return vec3;
}
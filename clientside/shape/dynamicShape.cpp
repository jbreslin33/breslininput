#include "dynamicShape.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../billboard/objectTitle.h"

#include <string.h>

DynamicShape::DynamicShape(Game* game, int ind, Vector3D* position, Vector3D* velocity,
						   Vector3D* rotation, std::string mesh)
:
	OgreShape         (game,ind,position,velocity,rotation,mesh)
{

}

DynamicShape::~DynamicShape()
{

}

void DynamicShape::processTick()
{

	//mObjectTitleString.clear(); //empty title string so it can be filled anew

	//call parent processTicks since you overrode them
	Move::processTick(); 
	Rotation::processTick();
	//OgreAnimation::processTick();
	
	//run billboard here for now.
	//mObjectTitle->setTitle(mObjectTitleString); //fill title with mObjectTitleString which we have appended from all states we want info on.
	//mObjectTitle->update(); //draw title for shape

}
void DynamicShape::interpolateTick(float renderTime)
{
	mRenderTime = renderTime;

	//call parent interpolateTicks since you overrode them
	Move::interpolateTick();
	Rotation::interpolateTick();
	//OgreAnimation::interpolateTick();
}

float Rotation::getDegreesToServer()
{
    Vector3D serverRotNew;

    serverRotNew.x = mServerFrame.mRot.x;
    serverRotNew.z = mServerFrame.mRot.z;

    serverRotNew.normalise();

    //calculate how far off we are from server
   // Quaternion toServer = getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew,Vector3::UNIT_Y);


    // convert to degrees
    //Real degreesToServer = toServer.getYaw().valueDegrees();
	float degreesToServer = getDegreesToSomething(serverRotNew);
	return degreesToServer;
}

void Rotation::calculateServerRotationSpeed()
{
    mServerRotOld.zero();
    mServerRotNew.zero();

    mServerRotOld.x = mServerFrame.mRotOld.x;
    mServerRotOld.z = mServerFrame.mRotOld.z;

    mServerRotNew.x = mServerFrame.mRot.x;
    mServerRotNew.z = mServerFrame.mRot.z;

    mServerRotNew.normalise();
    mServerRotOld.normalise();

    //calculate how far off we are from server
    //Quaternion toServer = getSceneNode()->getOrientation().zAxis().getRotationTo(mServerRotNew,Vector3::UNIT_Y);

    // convert to degrees
    //mDegreesToServer = toServer.getYaw().valueDegrees();

	mDegreesToServer = getDegreesToSomething(mServerRotNew);

    //calculate server rotation from last tick to new one
	
    //Quaternion serverRot = mGhost->getSceneNode()->getOrientation().zAxis().getRotationTo(mServerRotNew, Vector3::UNIT_Y);

    // convert to degrees
    //mServerRotSpeed = serverRot.getYaw().valueDegrees();
	mServerRotSpeed = mGhost->getDegreesToSomething(mServerRotNew);


    if(abs(mServerRotSpeed) < 0)
    {
		mServerRotSpeed = 0.0f;
    }
}
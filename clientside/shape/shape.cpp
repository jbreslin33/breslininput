#include "shape.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../billboard/objectTitle.h"

#include <string.h>

Shape::Shape(int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::SceneManager* mSceneMgr,
								 std::string mesh)
:
	Move    (),
	Rotation(),
	OgreAnimation     (),
	OgreShape         (ind,position,velocity,rotation,mSceneMgr,mesh)
{

}

Shape::~Shape()
{

}

void Shape::processTick()
{
	//call parent processTicks since you overrode them
	Move::processTick(); 
	Rotation::processTick();
	OgreAnimation::processTick();
	

}
void Shape::interpolateTick(float renderTime)
{

	mObjectTitleString.clear(); //empty title string so it can be filled anew

	mRenderTime = renderTime;

	//call parent interpolateTicks since you overrode them
	Move::interpolateTick();
	Rotation::interpolateTick();
	OgreAnimation::interpolateTick();

	//run billboard here for now.
	mObjectTitle->setTitle(mObjectTitleString); //fill title with mObjectTitleString which we have appended from all states we want info on.
	mObjectTitle->update(); //draw title for shape

	//run animations here for now.
	//float animSpeed = mRunSpeed * 1000/mRunSpeedMax;
	//float yVelocity = mServerFrame.mVelocity.y;
	//OgreAnimation::updateAnimations(yVelocity, renderTime, mServerFrame.mVelocity.isZero(), animSpeed);
}
#include "shape.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../billboard/objectTitle.h"

#include <string.h>

Shape::Shape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr,
								 std::string mesh)
:
	Move    (name,position,mSceneMgr,mesh),
	Rotation(name,position,mSceneMgr,mesh),
	OgreAnimation     (name,position,mSceneMgr,mesh),
	OgreShape         (name,position,mSceneMgr,mesh)
{
	mServerShape = NULL;
}

Shape::~Shape()
{
}

void Shape::processTick()
{
	mObjectTitleString.clear(); //empty title string so it can be filled anew

	Move::processTick(); 
	Rotation::processTick();

	mObjectTitle->setTitle(mObjectTitleString); //fill title with mObjectTitleString which we have appended from all states we want info on.

	mObjectTitle->update(); //draw title for shape

}
void Shape::interpolateTick(float renderTime)
{
	Move::interpolateTick(renderTime);
	Rotation::interpolateTick(renderTime);
	float animSpeed = mRunSpeed * 1000/mRunSpeedMax;

	float yVelocity = mServerFrame.mVelocity.y;
	OgreAnimation::updateAnimations(yVelocity, renderTime, mCommand.mStop, animSpeed);
}
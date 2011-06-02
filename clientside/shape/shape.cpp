#include "Shape.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../billboard/objectTitle.h"

#include <string.h>

Shape::Shape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr,
								 std::string mesh)
:
	ClientSideMove    (name,position,mSceneMgr,mesh),
	ClientSideRotation(name,position,mSceneMgr,mesh),
	OgreAnimation     (name,position,mSceneMgr,mesh),
	OgreShape         (name,position,mSceneMgr,mesh)
{
	mServerShape = NULL;

 	mPosition = position;
	mIndex  = 0;
   	mName = shapeName;
	mProcessedFrame = 0;
}

Shape::~Shape()
{
}

void Shape::processTick()
{
	mObjectTitleString.clear(); //empty title string so it can be filled anew
	
	ClientSideMove::processTick(); 
	ClientSideRotation::processTick();

	mObjectTitle->setTitle(mObjectTitleString); //fill title with mObjectTitleString which we have appended from all states we want info on.

	mObjectTitle->update(); //draw title for shape

}
void Shape::interpolateTick(float renderTime)
{
	ClientSideMove::interpolateTick(renderTime);
	ClientSideRotation::interpolateTick(renderTime);
	float animSpeed = mRunSpeed * 1000/mRunSpeedMax;

	float yVelocity = mServerFrame.mVelocity.y;
	OgreAnimation::updateAnimations(yVelocity, renderTime, mCommand.mStop, animSpeed);
}
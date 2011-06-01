#include "clientSideShape.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../billboard/objectTitle.h"

#include <string.h>

ClientSideShape::ClientSideShape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr,
								 std::string mesh)
:
	ClientSideMove    (name,position,mSceneMgr,mesh),
	ClientSideRotation(name,position,mSceneMgr,mesh),
	OgreAnimation     (name,position,mSceneMgr,mesh),
	OgreShape         (name,position,mSceneMgr,mesh)
{
	mServerShape = NULL;
}

ClientSideShape::~ClientSideShape()
{
}

void ClientSideShape::processTick()
{
	mObjectTitleString.clear(); //empty title string so it can be filled anew
	
	ClientSideMove::processTick(); 
	ClientSideRotation::processTick();

	mObjectTitle->setTitle(mObjectTitleString); //fill title with mObjectTitleString which we have appended from all states we want info on.

	mObjectTitle->update(); //draw title for shape

}
void ClientSideShape::interpolateTick(float renderTime)
{
	ClientSideMove::interpolateTick(renderTime);
	ClientSideRotation::interpolateTick(renderTime);
	float animSpeed = mRunSpeed * 1000/mRunSpeedMax;

	float yVelocity = mServerFrame.mVelocity.y;
	OgreAnimation::updateAnimations(yVelocity, renderTime, mCommand.mStop, animSpeed);
}
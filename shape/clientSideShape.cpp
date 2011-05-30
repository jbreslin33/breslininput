#include "clientSideShape.h"
#include "../tdreamsock/dreamSockLog.h"

#include <string>

ClientSideShape::ClientSideShape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr,
								 std::string mesh)
 : ClientSideMove(name,position,mSceneMgr,mesh),
 ClientSideRotation(name,position,mSceneMgr,mesh),
 OgreAnimation(name,position,mSceneMgr,mesh),
 OgreShape(name,position,mSceneMgr,mesh)
{
	mServerShape = NULL;
}

ClientSideShape::~ClientSideShape()
{
}

void ClientSideShape::processTick()
{
	ClientSideMove::processTick();
	ClientSideRotation::processTick();

}
void ClientSideShape::interpolateTick(float renderTime)
{
	ClientSideMove::interpolateTick(renderTime);
	ClientSideRotation::interpolateTick(renderTime);
	float animSpeed = mRunSpeed * 1000/mRunSpeedMax;

	float yVelocity = mServerFrame.mVelocity.y;
	OgreAnimation::updateAnimations(yVelocity, renderTime, mCommand.mStop, animSpeed);
}
#include "clientSideShape.h"
#include "../tdreamsock/dreamSockLog.h"

#include <string>

ClientSideShape::ClientSideShape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr,
								 std::string mesh)
 : ClientSideMove(name,position,mSceneMgr,mesh),
 ClientSideRotation(name,position,mSceneMgr,mesh),
 OgreAnimation(name,position,mSceneMgr,mesh)
{
	LogString("ClientSideShape Constructor");
//setupAnimations(mEntity);
	mServerShape = NULL;
	
}

ClientSideShape::~ClientSideShape()
{
}

void ClientSideShape::processTick()
{
	//Move::processTick();
	//Rotation::processTick();
}
void ClientSideShape::interpolateTick(float renderTime)
{
	//Move::interpolateTick(renderTime);
	//Rotation::interpolateTick(renderTime);
}
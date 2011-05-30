#include "serverSideShape.h"
#include "../tdreamsock/dreamSockLog.h"

#include <string>

ServerSideShape::ServerSideShape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr,
								 std::string mesh)
 : ServerSideMove(name,position,mSceneMgr,mesh),
 ServerSideRotation(name,position,mSceneMgr,mesh),
 OgreShape(name,position,mSceneMgr,mesh)
{
	mServerShape = NULL;
}

ServerSideShape::~ServerSideShape()
{
}

void ServerSideShape::processTick()
{
	ServerSideMove::processTick();
	ServerSideRotation::processTick();

}

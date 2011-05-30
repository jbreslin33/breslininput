#include "serverSideShape.h"
#include "../tdreamsock/dreamSockLog.h"

#include <string>

ServerSideShape::ServerSideShape(std::string name, Vector3D* position, Ogre::Root* root)
:
	ServerSideMove    (name,position,root),
	ServerSideRotation(name,position,root),
	OgreShape		  (name,position,root)
{

}

ServerSideShape::~ServerSideShape()
{
}

void ServerSideShape::processTick()
{
	//LogString("serverSideshape tick");
	ServerSideMove::processTick();
	ServerSideRotation::processTick();

}

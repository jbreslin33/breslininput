#include "clientSideShape.h"
#include "../tdreamsock/dreamSock.h"

ClientSideShape::ClientSideShape(Ogre::SceneManager* sceneMgr, std::string shapeName,int x, int y, int z,std::string meshName) : Shape(sceneMgr,shapeName,x,y,z,meshName)
{

}

ClientSideShape::~ClientSideShape()
{
}

#include "clientSideShape.h"

#include "../math/vector3D.h"


ClientSideShape::ClientSideShape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh)
 : OgreShape(name,position,mSceneMgr,mesh)
{
	
}

ClientSideShape::~ClientSideShape()
{
}


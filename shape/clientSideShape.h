#ifndef CLIENTSIDESHAPE_H
#define CLIENTSIDESHAPE_H

#include "ogreShape.h"


//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class ClientSideShape : public OgreShape
{

public:
ClientSideShape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~ClientSideShape();



};

#endif


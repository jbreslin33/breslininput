#ifndef CLIENTSIDESHAPE_H
#define CLIENTSIDESHAPE_H

#include "shape.h"

#include "Ogre.h"
#include "OIS.h"

using namespace Ogre;

class ClientSideShape : public Shape
{
public:

ClientSideShape(Ogre::SceneManager* mSceneMgr, std::string name, int xPos, int yPos, int zPos,std::string meshName); 
~ClientSideShape();

};

#endif


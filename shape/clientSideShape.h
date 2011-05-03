#ifndef CLIENTSIDESHAPE_H
#define CLIENTSIDESHAPE_H

#include "shape.h"
#include "../client/clientSideClient.h"

#include "Ogre.h"
#include "OIS.h"

using namespace Ogre;

class ClientSideShape : public Shape
{
public:

ClientSideShape(ClientSideClient* client,Ogre::SceneManager* mSceneMgr, std::string name, int xPos, int yPos, int zPos,std::string meshName); 
~ClientSideShape();

ClientSideClient* mClient;

};

#endif


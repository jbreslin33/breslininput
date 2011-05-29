#ifndef CLIENTSIDESHAPE_H
#define CLIENTSIDESHAPE_H

//parents
#include "../move/clientSideMove.h"
#include "../rotation/clientSideRotation.h"
#include "../animation/ogreAnimation.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

#include <string>

class ClientSideShape : public ClientSideMove , public ClientSideRotation, public OgreAnimation
{

public:
ClientSideShape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~ClientSideShape();

ClientSideShape* mServerShape;

void processTick();
void interpolateTick(float renderTime);

};

#endif


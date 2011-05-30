#ifndef SERVERSIDESHAPE_H
#define SERVERSIDESHAPE_H

//parents
#include "../move/serverSideMove.h"
#include "../rotation/serverSideRotation.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

#include <string>

class ServerSideShape : public ServerSideMove , public ServerSideRotation
{

public:
ServerSideShape(std::string name, Vector3D* position, Ogre::Root* root);
~ServerSideShape();

ServerSideShape* mServerShape;

void processTick();

};

#endif
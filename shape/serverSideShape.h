#ifndef SERVERSIDESHAPE_H
#define SERVERSIDESHAPE_H

//parents
#include "../rotation/serverSideRotation.h"
#include "../move/serverSideMove.h"
#include "../jump/serverSideJump.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

#include <string>

class Client;

class ServerSideShape
	:
	public ServerSideMove ,
	public ServerSideRotation ,
	public ServerSideJump
{

public:
ServerSideShape(std::string name, Vector3D* position, Ogre::Root* root);
~ServerSideShape();

void processTick();

//associated client if any
Client* mClient;



};

#endif
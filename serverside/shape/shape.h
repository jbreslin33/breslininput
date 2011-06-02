#ifndef SHAPE_H
#define SHAPE_H

//parents
#include "../../serverside/rotation/rotation.h"
#include "../../serverside/move/move.h"
#include "../../serverside/jump/jump.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

#include <string>

class Client;

class Shape
	:
	public Move ,
	public Rotation ,
	public Jump
{

public:
Shape(std::string name, Vector3D* position, Ogre::Root* root);
~Shape();

//ticks
void processTick();

//associated client if any
Client* mClient;




};

#endif
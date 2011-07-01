#ifndef SHAPE_H
#define SHAPE_H

//parents
#include "../../serverside/rotation/rotation.h"
#include "../../serverside/move/move.h"
#include "../../serverside/jump/jump.h"
#include "../../serverside/ai/ai.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

#include <string>

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8

class Client;

class Shape
	:
	public Move ,
	public Rotation ,
	public Jump,
	public AI
{

public:
Shape(Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::Root* root,unsigned int index);
~Shape();

//ticks
void processTick();

void setKeyDirection();

//associated client if any
Client* mClient;




};

#endif
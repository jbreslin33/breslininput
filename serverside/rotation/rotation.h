#ifndef ROTATION_H
#define ROTATION_H

//parents
#include "../../serverside/shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class ServerSideRotationStateMachine;
class ServerSideShape;

class ServerSideRotation :  public virtual OgreShape
{

public:

ServerSideRotation(std::string name, Vector3D* position, Ogre::Root* root);
~ServerSideRotation();

//movement
void processTick    ();
void setKeyDirection();
ServerSideRotationStateMachine* mRotationStateMachine;

//keys
Vector3 mKeyDirection;
Vector3 mGoalDirection;
};

#endif


#ifndef SERVERSIDEROTATION_H
#define SERVERSIDEROTATION_H

//parents
#include "rotation.h"
#include "../shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class ServerSideRotationStateMachine;
class ServerSideShape;

class ServerSideRotation :  public virtual OgreShape, public Rotation
{

public:

ServerSideRotation(std::string name, Vector3D* position, Ogre::Root* root);
~ServerSideRotation();

//movement
void processTick    ();

ServerSideRotationStateMachine* mRotationStateMachine;
};

#endif


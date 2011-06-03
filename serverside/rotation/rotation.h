#ifndef ROTATION_H
#define ROTATION_H

//parents
#include "../../serverside/shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class RotationStateMachine;
class Shape;

class Rotation :  public virtual OgreShape
{

public:

Rotation(std::string name, Vector3D* position, Ogre::Root* root);
~Rotation();

//movement
void processTick    ();
void setKeyDirection();
RotationStateMachine* mRotationStateMachine;


};

#endif


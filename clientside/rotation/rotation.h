#ifndef ROTATION_H
#define ROTATION_H

//parents
#include "../../clientside/shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class RotationStateMachine;
class Shape;

class Rotation :  public virtual OgreShape
{

public:

Rotation();
~Rotation();

//movement
void processTick    ();

void interpolateTick    ();
Real getDegreesToServer();
void calculateServerRotationSpeed();

RotationStateMachine* mRotationProcessTickStateMachine;
RotationStateMachine* mRotationInterpolateTickStateMachine;

//vars
float mServerRotSpeed;
float mTurnSpeed;

float mRotInterpLimitHigh;
float mRotInterpLimitLow;
float mRotInterpIncrease; 
float mRotInterpDecrease;

//rotation
Ogre::Vector3 mServerRotOld;
Ogre::Vector3 mServerRotNew;
Ogre::Real    mDegreesToServer;
};

#endif


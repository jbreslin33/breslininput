#ifndef ROTATION_H
#define ROTATION_H

#include "rotation.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class RotationStateMachine;

class Rotation
{

public:

Rotation();
~Rotation();

//movement
void processTick    ();

void interpolateTick    (float renderTime);

//rotation state machine
RotationStateMachine* mRotationStateMachine;

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


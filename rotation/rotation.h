#ifndef ROTATION_H
#define ROTATION_H

#include "rotation.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class ClientSidePlayer;
class RotationStateMachine;

class Rotation
{

public:

Rotation(ClientSidePlayer* player);
~Rotation();

//movement
void processTick    ();

void interpolateTick    (float renderTime);

ClientSidePlayer* mPlayer;

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


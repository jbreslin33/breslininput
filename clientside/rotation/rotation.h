#ifndef ROTATION_H
#define ROTATION_H

//parents
#include "../../clientside/shape/shape.h"

class Command;
class RotationStateMachine;
class DynamicShape;

class Rotation :  public Shape
{

public:

Rotation();
~Rotation();

//movement
void processTick    ();

void interpolateTick    ();
float getDegreesToServer();
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
Vector3D mServerRotOld;
Vector3D mServerRotNew;
float    mDegreesToServer;
};

#endif


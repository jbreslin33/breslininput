#ifndef MOVE_H
#define MOVE_H

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class MoveStateMachine;
class Command;

class Move
{

public:

Move();
~Move();

//ticks
void processTick    ();
void interpolateTick    (float renderTime);
void calculateVelocity(Command* command, float frametime);

//move state machine
MoveStateMachine*   mMoveStateMachine;

float mRunSpeed;
float mRunSpeedMax;
float mTurnSpeed;
float mPosInterpLimitHigh;
float mPosInterpLimitLow;
float mPosInterpFactor;

//deltas
float mDeltaX;
float mDeltaZ;
float mDeltaY;

float mDeltaPosition;


};

#endif


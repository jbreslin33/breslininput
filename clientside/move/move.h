#ifndef MOVE_H
#define MOVE_H

//parents
#include "../../clientside/shape/shape.h"

class Command;
class MoveStateMachine;

class Move : public Shape
{

public:

Move();
~Move();

//ticks
void processTick    ();
void interpolateTick    ();
void calculateDeltaPosition();

MoveStateMachine* mMoveProcessTickStateMachine;
MoveStateMachine* mMoveInterpolateTickStateMachine;

//thresholds
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


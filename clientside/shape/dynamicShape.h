#ifndef DYNAMICSHAPE_H
#define DYNAMICSHAPE_H

#include "ogreShape.h"

#include <string>

//i want a dynamic shape...this should be dynamic shape shape should hide ogreshape...

class DynamicShapeStateMachine;

class DynamicShape : public OgreShape //, public OgreAnimation
{

public:
DynamicShape(Game* game, int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, std::string mesh);
~DynamicShape();

//ticks
void processTick();
void interpolateTick(float renderTime);

float getDegreesToServer();
void calculateServerRotationSpeed();

/////move
void calculateDeltaPosition();

DynamicShapeStateMachine* mMoveProcessTickStateMachine;
DynamicShapeStateMachine* mMoveInterpolateTickStateMachine;

//rotation states
DynamicShapeStateMachine* mRotationProcessTickStateMachine;
DynamicShapeStateMachine* mRotationInterpolateTickStateMachine;

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

////rotation
//vars
float mServerRotSpeed;

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


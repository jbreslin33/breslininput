#ifndef DYNAMICSHAPE_H
#define DYNAMICSHAPE_H

#include "shape.h"

//command
#include "../../command/command.h"
#define COMMAND_HISTORY_SIZE		64

#include <string>

//i want a dynamic shape...this should be dynamic shape shape should hide ogreshape...
class Game;
class DynamicShapeStateMachine;

class DynamicShape : public Shape //, public OgreAnimation
{

public:
DynamicShape(Game* game,int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation);
~DynamicShape();

//virtual
virtual void     yaw                  (float amountToYaw, bool converToDegree   ) = 0;
virtual float    getDegreesToSomething(Vector3D something                       ) = 0;
virtual void     translate            (Vector3D translateVector, int perspective) = 0;

//dynamic
//run speed
float mRunSpeed;

float mRunSpeedMax;

Command	mServerFrame;					// the latest frame from server
Command mCommandToRunOnShape;

float mRenderTime;


DynamicShape* mGhost;

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

std::string mName;

std::string getName() { return mName; }
Game* mGame;

};

#endif


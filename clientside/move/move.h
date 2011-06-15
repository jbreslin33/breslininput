#ifndef MOVE_H
#define MOVE_H

//parents
#include "../../clientside/shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class MoveStateMachine;

class Move : public virtual OgreShape
{

public:

Move(Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::SceneManager* mSceneMgr, std::string mesh);
~Move();

//ticks
void processTick    ();
void interpolateTick    ();
void calculateDeltaPosition();

MoveStateMachine* mMoveProcessTickStateMachine;
MoveStateMachine* mMoveInterpolateTickStateMachine;

//run speed
float mRunSpeed;

//thresholds
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


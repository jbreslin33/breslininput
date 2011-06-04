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

Move(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~Move();

//ticks
void processTick    ();
void interpolateTick    (float renderTime);

MoveStateMachine* mMoveStateMachine;

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


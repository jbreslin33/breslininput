#ifndef CLIENTSIDEMOVE_H
#define CLIENTSIDEMOVE_H

#include "../shape/clientSideShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class MoveStateMachine;
class Command;

class ClientSideMove : public virtual ClientSideShape
{

public:

ClientSideMove(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~ClientSideMove();

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


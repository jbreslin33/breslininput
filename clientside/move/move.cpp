#include "move.h"
#include "../../tdreamsock/dreamSockLog.h"

#include <string>

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//move states
#include "moveStateMachine.h"
#include "moveStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16

Move::Move()
 : Shape()
{
    //thresholds

    mPosInterpLimitHigh = 8.0; //how far away from server till we try to catch up
    mPosInterpLimitLow  = 2.0; //how close to server till we are in sync
    mPosInterpFactor    = 4.0;


	//deltas
	mDeltaX        = 0.0; 
	mDeltaY		   = 0.0;
	mDeltaZ        = 0.0;
	mDeltaPosition = 0.0;

	//move processTick states
	mMoveProcessTickStateMachine = new MoveStateMachine(this);    //setup the state machine
	mMoveProcessTickStateMachine->setCurrentState      (Normal_ProcessTick_Move::Instance());
	mMoveProcessTickStateMachine->setPreviousState     (Normal_ProcessTick_Move::Instance());
	mMoveProcessTickStateMachine->setGlobalState       (Global_ProcessTick_Move::Instance());

	//move interpolateTick states
	mMoveInterpolateTickStateMachine = new MoveStateMachine(this);    //setup the state machine
	mMoveInterpolateTickStateMachine->setCurrentState      (Normal_InterpolateTick_Move::Instance());
	mMoveInterpolateTickStateMachine->setPreviousState     (Normal_InterpolateTick_Move::Instance());
	//mMoveInterpolateTickStateMachine->setGlobalState       (Global_InterpolateTick_Move::Instance());
	mMoveInterpolateTickStateMachine->setGlobalState       (NULL);
}

Move::~Move()
{
	delete mMoveProcessTickStateMachine;
	delete mMoveInterpolateTickStateMachine;
}

void Move::processTick()
{
    mMoveProcessTickStateMachine->update();
}

void Move::interpolateTick()
{
	mMoveInterpolateTickStateMachine->update();
}

void Move::calculateDeltaPosition()
{
	mDeltaX = mServerFrame.mOrigin.x - getPosition().x;
    mDeltaY = mServerFrame.mOrigin.y - getPosition().y;
    mDeltaZ = mServerFrame.mOrigin.z - getPosition().z;

    //distance we are off from server
    mDeltaPosition = sqrt(pow(mDeltaX, 2) + pow(mDeltaY, 2) +  pow(mDeltaZ, 2));
}
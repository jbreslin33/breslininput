#include "move.h"
#include "../tdreamsock/dreamSockLog.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16

Move::Move()
{
	mRunSpeed     = 0.0;
	mRunSpeedMax  = 200.0;
	//mAccel        = 50.0
	
	mPosInterpLimitHigh = 8.0; //how far away from server till we try to catch up
	mPosInterpLimitLow  = 2.0; //how close to server till we are in sync
	mPosInterpFactor    = 4.0;

	//deltas
	mDeltaX        = 0.0; 
	mDeltaZ		   = 0.0;
	mDeltaY        = 0.0;
	mDeltaPosition = 0.0;
}

Move::~Move()
{
}

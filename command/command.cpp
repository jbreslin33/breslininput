#include "command.h"

Command::Command()
{
	mOrigin.x = 0.0;
	mOrigin.y = 0.0;
	mOrigin.z = 0.0;

	mVelocity.x = 0.0;
	mVelocity.y = 0.0;
	mVelocity.z = 0.0;

	mPredictedOrigin.x = 0.0;
	mPredictedOrigin.y = 0.0;
	mPredictedOrigin.z = 0.0;

	mVelocityOld.x = 0.0;
	mVelocityOld.y = 0.0;
	mVelocityOld.z = 0.0;

	mKey = 0;

	mMilliseconds = 0;

	mCatchup    = false;
	mCatchupRot = false;
	mStop       = true;
}

Command::~Command()
{

}

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

	mRotOld.x = 0.0;
	mRotOld.y = 0.0;
	mRotOld.z = 0.0;

	mKey = 0;

	mMilliseconds = 0;

	mCatchup    = false;
	mCatchupRot = false;
	mStop       = true;

	mClientFrametime = 0.0f;
}

Command::~Command()
{

}

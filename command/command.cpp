#include "command.h"

Command::Command()
{
	mOrigin.x = 0.0;
	mOrigin.y = 0.0;
	mOrigin.z = 0.0;

	mOriginOld.x = 0.0;
	mOriginOld.y = 0.0;
	mOriginOld.z = 0.0;

	mVelocity.x = 0.0;
	mVelocity.y = 0.0;
	mVelocity.z = 0.0;

	mRotOld.x = 0.0;
	mRotOld.y = 0.0;
	mRotOld.z = 0.0;

	mKey = 0;
	mMilliseconds = 0;
	mMillisecondsTotal = 0;
	mClientFrametime = 0.0f;
}

Command::~Command()
{
}

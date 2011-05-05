#ifndef COMMAND_H
#define COMMAND_H

#include "../math/vector3D.h"

class Command
{
public:
Command();
~Command();
	
	Vector3D mOrigin; //finish origin of frame/tick
	Vector3D mVelocity;	//velocity during frame/tick

	int mKey; //key pressed
	int mMilliseconds; //not used

	Vector3D	mPredictedOrigin;

	bool        mCatchup;
	bool        mStop;
};
#endif

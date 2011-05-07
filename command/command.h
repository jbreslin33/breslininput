#ifndef COMMAND_H
#define COMMAND_H

#include "../math/vector3D.h"

class Command
{
public:
Command();
~Command();
	
Vector3D mOrigin;            //finish origin of frame/tick
Vector3D mVelocity;	         //velocity during frame/tick
Vector3D mPredictedOrigin;
Vector3D mVelocityOld;	//velocity during last frame/tick

float mRotSpeed;

int mKey;               //key pressed
int mMilliseconds;      //not used

bool mCatchup;
bool mCatchupRot;
bool mStop;
};
#endif

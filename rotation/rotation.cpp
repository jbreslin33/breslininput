#include "rotation.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16

Rotation::Rotation()
{
	mTurnSpeed = 250.0;

	mRotInterpLimitHigh = 6.0; //how far away from server till we try to catch up
	mRotInterpLimitLow  = 4.0; //how close to server till we are in sync
	mRotInterpIncrease  = 1.20f; //rot factor used to catchup to server
    mRotInterpDecrease  = 0.80f; //rot factor used to allow server to catchup to client

	//rotation
	mServerRotOld  = Ogre::Vector3::ZERO;
	mServerRotNew  = Ogre::Vector3::ZERO;
	mDegreesToServer = 0.0;

}

Rotation::~Rotation()
{
}

#ifndef CLIENTSIDECOMMAND_H
#define CLIENTSIDECOMMAND_H

#include "command.h"

#include "../math/vector3D.h"
#include "Ogre.h"

using namespace Ogre;

class ClientSideCommand : public Command
{
public:
ClientSideCommand();
~ClientSideCommand();
	
	Vector3D mOrigin; //finish origin of frame/tick
	Vector3D mVelocity;	//velocity during frame/tick

	int mKey; //key pressed
	int mMilliseconds; //not used

	Vector3D	mPredictedOrigin;

	bool        mCatchup;
	bool        mStop;

};
#endif

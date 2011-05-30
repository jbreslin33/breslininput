#include "serverSideShape.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../client/client.h"

#include <string>

ServerSideShape::ServerSideShape(std::string name, Vector3D* position, Ogre::Root* root)
:
	ServerSideMove    (name,position,root),
	ServerSideRotation(name,position,root),
	OgreShape		  (name,position,root)
{
	//client if this shape has associated with it
	mClient = NULL;
}

ServerSideShape::~ServerSideShape()
{
}

void ServerSideShape::processTick()
{
	//LogString("serverSideshape tick");
	ServerSideMove::processTick();
	ServerSideRotation::processTick();

	if (mClient != NULL)
	{
		int f = mClient->GetIncomingSequence() & (COMMAND_HISTORY_SIZE-1);
		mProcessedFrame = f;
	}
}

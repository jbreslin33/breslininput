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
	ServerSideRotation::processTick();

	ServerSideMove::processTick();




	//set all vars to be sent off to clients playing on internets
    mCommand.mVelocity.x = mSceneNode->getPosition().x - mCommand.mOrigin.x;
    mCommand.mVelocity.z = mSceneNode->getPosition().z - mCommand.mOrigin.z;
    mCommand.mVelocity.y = mSceneNode->getPosition().y - mCommand.mOrigin.y;

    mCommand.mOrigin.x = mSceneNode->getPosition().x;
    mCommand.mOrigin.z = mSceneNode->getPosition().z;
    mCommand.mOrigin.y = mSceneNode->getPosition().y;

    mCommand.mRot.x = mSceneNode->getOrientation().zAxis().x;
    mCommand.mRot.z = mSceneNode->getOrientation().zAxis().z;

	//set mProcessed frame if their is a client controlling this shape
	if (mClient != NULL)
	{
		int f = mClient->GetIncomingSequence() & (COMMAND_HISTORY_SIZE-1);
		mProcessedFrame = f;
	}
}

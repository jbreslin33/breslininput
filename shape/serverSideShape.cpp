#include "serverSideShape.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../client/client.h"

#include <string>

ServerSideShape::ServerSideShape(std::string name, Vector3D* position, Ogre::Root* root)
:
	ServerSideRotation(name,position,root),
	ServerSideMove    (name,position,root),
	ServerSideJump    (name,position,root),
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
	setKeyDirection();

	//don't do any rotation if there is no mKey set
	//should we though? we do it for movement below
	//does rotation not get to do the same?
	//is this why the rotation gets off???
    if (mCommand.mKey != 0)
	{
		ServerSideRotation::processTick();
	}

	//even though there is no mKey set we still need to move as
	//brian has put in a deceleration factor so let's let it run...
	ServerSideMove::processTick();

	//jump is a good work in progress but it's now it's own thing
	ServerSideJump::processTick();

	//set all vars to be sent off to clients playing on internets
	//none of this actually moves anything on server it is what is
	//going to be sent to clients so it's not in move or rotation.
	//though it could be i guess, velocity and position could go in move
	//and mRot could go in rotation.
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

#include "clientSidePlayer.h"

//just using this for logstring need to fix that
#include "../tdreamsock/dreamSock.h"

//client,shape,animation combo
#include "../client/clientSideClient.h"
#include "../shape/ogreShape.h"

//move states
#include "../move/move.h"
#include "../rotation/rotation.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8

ClientSidePlayer::ClientSidePlayer(std::string name, ClientSideClient* client, OgreShape* shape) : Player(name)
{
	mClient   = client;
	mShape    = shape;
	mMove     = new Move(this);
	mRotation = new Rotation(this);
}

ClientSidePlayer::~ClientSidePlayer()
{
}

void ClientSidePlayer::processTick()
{
	mMove->processTick();
	mRotation->processTick();
}
void ClientSidePlayer::interpolateTick(float renderTime)
{
	mMove->interpolateTick(renderTime);
	mRotation->interpolateTick(renderTime);
}

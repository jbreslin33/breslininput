#include "clientSidePlayer.h"

//client,shape,animation combo
#include "../shape/ogreShape.h"

//move states
#include "../move/move.h"
#include "../rotation/rotation.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8

ClientSidePlayer::ClientSidePlayer(std::string name, OgreShape* shape) : Player(name), Move(this), Rotation(this)
{
	mClientSideServerPlayer = NULL;
	mIndex  = 0;
	mShape    = shape;
}

ClientSidePlayer::~ClientSidePlayer()
{
}

void ClientSidePlayer::processTick()
{
	Move::processTick();
	Rotation::processTick();
}
void ClientSidePlayer::interpolateTick(float renderTime)
{
	Move::interpolateTick(renderTime);
	Rotation::interpolateTick(renderTime);
}

#include "serverSideMove.h"
#include "../tdreamsock/dreamSockLog.h"

#include <string>

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//move states
#include "serverSideMoveStateMachine.h"
#include "serverSideMoveStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16

ServerSideMove::ServerSideMove(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr,
							   std::string mesh)
 : OgreShape(name,position,mSceneMgr,mesh), Move() 
{
	//move states
	mMoveStateMachine = new ServerSideMoveStateMachine(this);    //setup the state machine
	mMoveStateMachine->setCurrentState      (Normal_Move::Instance());
	mMoveStateMachine->setPreviousState     (Normal_Move::Instance());
	mMoveStateMachine->setGlobalState       (NULL);
}

ServerSideMove::~ServerSideMove()
{
}

void ServerSideMove::processTick()
{
	mDeltaX = mServerFrame.mOrigin.x - getSceneNode()->getPosition().x;
    mDeltaZ = mServerFrame.mOrigin.z - getSceneNode()->getPosition().z;
    mDeltaY = mServerFrame.mOrigin.y - getSceneNode()->getPosition().y;

    //distance we are off from server
    mDeltaPosition = sqrt(pow(mDeltaX, 2) + pow(mDeltaZ, 2) + pow(mDeltaY, 2));

    // if server has come to a stop
    if(mServerFrame.mVelocity.x == 0.0 && mServerFrame.mVelocity.z == 0.0
		&& mServerFrame.mVelocity.y == 0.0)
    {
		mCommand.mStop = true;
    }
    else //server still moving
    {
		mCommand.mStop = false;
    }
    mMoveStateMachine->update();
}

void ServerSideMove::calculateVelocity(Command* command, float frametime)
{
	command->mVelocity.x = 0.0f;
    command->mVelocity.z = 0.0f;

    if(command->mKey & KEY_UP)
    {
		command->mVelocity.z += -1;
    }

    if(command->mKey & KEY_DOWN)
    {
		command->mVelocity.z += 1;
    }
    if(command->mKey & KEY_LEFT)
    {
		command->mVelocity.x += -1;
    }
    if(command->mKey & KEY_RIGHT)
    {
        command->mVelocity.x += 1;
    }

	float length = sqrt(pow(command->mVelocity.x, 2) + pow(command->mVelocity.z, 2));
    if(length != 0.0)
    {
		command->mVelocity.x = command->mVelocity.x/length * mRunSpeed * frametime;
        command->mVelocity.z = command->mVelocity.z/length * mRunSpeed * frametime;
    }
}


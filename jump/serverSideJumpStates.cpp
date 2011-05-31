#include "serverSideJumpStates.h"
#include "serverSideJumpStateMachine.h"

#include "serverSideJump.h"

//key defines prob should be changed to a variable if possible
#define KEY_SPACE				16

Normal_Jump* Normal_Jump::Instance()
{
  static Normal_Jump instance;
  return &instance;
}
void Normal_Jump::enter(ServerSideJump* jump)
{
	jump->startJump();
}
void Normal_Jump::execute(ServerSideJump* jump)
{
	jump->mSceneNode->translate(0, jump->mCommand.mClientFrametime * jump->mVerticalVelocity, 0, Node::TS_LOCAL);
    jump->mVerticalVelocity -= jump->mGravity * jump->mCommand.mClientFrametime;

    if(jump->mSceneNode->getPosition().y < 0.0)
    {
		jump->mSceneNode->setPosition(jump->mSceneNode->getPosition().x, 0.0, jump->mSceneNode->getPosition().z);
        jump->mVerticalVelocity = 0.0;
		jump->mJumpStateMachine->changeState(No_Jump::Instance());
    }
}
void Normal_Jump::exit(ServerSideJump* jump)
{
}

No_Jump* No_Jump::Instance()
{
	static No_Jump instance;
	return &instance;
}
void No_Jump::enter(ServerSideJump* jump)
{
}
void No_Jump::execute(ServerSideJump* jump)
{
    if(jump->mCommand.mKey & KEY_SPACE) 
    {
		jump->mJumpStateMachine->changeState(Normal_Jump::Instance());
    }
}
void No_Jump::exit(ServerSideJump* jump)
{
}


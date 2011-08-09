#include "jumpStates.h"
#include "../tdreamsock/dreamSockLog.h"

#include "jumpStateMachine.h"

#include "jump.h"

//key defines prob should be changed to a variable if possible
#define KEY_SPACE				16

Normal_Jump* Normal_Jump::Instance()
{
  static Normal_Jump instance;
  return &instance;
}
void Normal_Jump::enter(Jump* jump)
{
	jump->startJump();
}
void Normal_Jump::execute(Jump* jump)
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
void Normal_Jump::exit(Jump* jump)
{
}

No_Jump* No_Jump::Instance()
{
	static No_Jump instance;
	return &instance;
}
void No_Jump::enter(Jump* jump)
{
}
void No_Jump::execute(Jump* jump)
{
    if(jump->mCommand.mKey & KEY_SPACE) 
    {
		jump->mJumpStateMachine->changeState(Normal_Jump::Instance());
    }
}
void No_Jump::exit(Jump* jump)
{
}


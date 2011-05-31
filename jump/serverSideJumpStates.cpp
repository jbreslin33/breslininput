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

}
void Normal_Jump::execute(ServerSideJump* jump)
{
	float clientFrametime;
    clientFrametime = jump->mCommand.mMilliseconds / 1000.0f;;

    if(jump->mCommand.mKey & KEY_SPACE) 
    {
		if(!jump->mJumping)
		{
			jump->startJump();
	    }
    }

    if(jump->mJumping)
    {
        jump->mSceneNode->translate(0, clientFrametime * jump->mVerticalVelocity, 0, Node::TS_LOCAL);
        jump->mVerticalVelocity -= jump->mGravity * clientFrametime;

        if(jump->mSceneNode->getPosition().y < 0.0)
        {
            jump->mSceneNode->setPosition(jump->mSceneNode->getPosition().x, 0.0, jump->mSceneNode->getPosition().z);
            jump->mVerticalVelocity = 0.0;
            jump->mJumping = false;
        }
	}
}
void Normal_Jump::exit(ServerSideJump* jump)
{
}

Catchup_Jump* Catchup_Jump::Instance()
{
	static Catchup_Jump instance;
	return &instance;
}
void Catchup_Jump::enter(ServerSideJump* jump)
{
}
void Catchup_Jump::execute(ServerSideJump* jump)
{
	
}
void Catchup_Jump::exit(ServerSideJump* jump)
{
}


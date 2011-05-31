#include "serverSideMoveStates.h"
#include "serverSideMoveStateMachine.h"

#include "serverSideMove.h"

#define MAX_RUN_SPEED 200           // character running speed in units per second

Normal_Move* Normal_Move::Instance()
{
  static Normal_Move instance;
  return &instance;
}
void Normal_Move::enter(ServerSideMove* move)
{

}
void Normal_Move::execute(ServerSideMove* move)
{
	float clientFrametime;
    clientFrametime = move->mCommand.mMilliseconds / 1000.0f;;

    if (move->mCommand.mKey == 0)
	{
		if(move->mRunSpeed > 0.0)
		{
			move->mRunSpeed -= move->mRunDecel;
		}
        else
		{
           move->mRunSpeed = 0.0;
		}
    }
	else
	{
        if(move->mRunSpeed < MAX_RUN_SPEED)
		{
			move->mRunSpeed += move->mRunAccel;
		}
	}

    // move in current body direction (not the goal direction)
    move->mSceneNode->translate(0, 0, clientFrametime * move->mRunSpeed,Node::TS_LOCAL);
}
void Normal_Move::exit(ServerSideMove* move)
{
}

Catchup_Move* Catchup_Move::Instance()
{
	static Catchup_Move instance;
	return &instance;
}
void Catchup_Move::enter(ServerSideMove* move)
{
}
void Catchup_Move::execute(ServerSideMove* move)
{
	
}
void Catchup_Move::exit(ServerSideMove* move)
{
}


#include "moveStates.h"
#include "moveStateMachine.h"

#include "move.h"

#include "../../command/command.h"

#define MAX_RUN_SPEED 200           // character running speed in units per second

Normal_Move* Normal_Move::Instance()
{
  static Normal_Move instance;
  return &instance;
}
void Normal_Move::enter(Move* move)
{

}
void Normal_Move::execute(Move* move)
{
    if (move->mKeyDirection.isZeroLength()) 
	{
		if(move->mRunSpeed > 0.0) //Decelerate_Move
		{
			move->mMoveStateMachine->changeState(Decelerate_Move::Instance());
			return;
		}
        else //No_Move
		{
			move->mMoveStateMachine->changeState(No_Move::Instance());
			return;
		}
    }
	else 
	{
        if(move->mRunSpeed < MAX_RUN_SPEED) //Accelerate_Move
		{
			move->mMoveStateMachine->changeState(Accelerate_Move::Instance());
			return;
		}
	}

	// move in current body direction (not the goal direction)
	move->mSceneNode->translate(0, 0, move->mCommand.mClientFrametime * move->mRunSpeed,Node::TS_LOCAL);
}
void Normal_Move::exit(Move* move)
{
}

No_Move* No_Move::Instance()
{
	static No_Move instance;
	return &instance;
}
void No_Move::enter(Move* move)
{
}
void No_Move::execute(Move* move)
{
    if (move->mKeyDirection.isZeroLength()) 
	{
		if(move->mRunSpeed > 0.0) //Decelerate_Move
		{
			move->mMoveStateMachine->changeState(Decelerate_Move::Instance());
			return;
		}
        else //No_Move
		{
           move->mRunSpeed = 0.0;
		}
    }
	else 
	{
        if(move->mRunSpeed < MAX_RUN_SPEED) //Accelerate_Move
		{
			move->mMoveStateMachine->changeState(Accelerate_Move::Instance());
			return;
		}
		else //Normal_Move 
		{
			move->mMoveStateMachine->changeState(Normal_Move::Instance());
			return;
		}
	}
}
void No_Move::exit(Move* move)
{
}

Accelerate_Move* Accelerate_Move::Instance()
{
	static Accelerate_Move instance;
	return &instance;
}
void Accelerate_Move::enter(Move* move)
{
}
void Accelerate_Move::execute(Move* move)
{
    if (move->mKeyDirection.isZeroLength()) 
	{
		if(move->mRunSpeed > 0.0) //Decelerate_Move
		{
			move->mMoveStateMachine->changeState(Decelerate_Move::Instance());
			return;
		}
        else //No_Move
		{
			move->mMoveStateMachine->changeState(No_Move::Instance());
			return;
		}
    }
	else 
	{
        if(move->mRunSpeed < MAX_RUN_SPEED) //Accelerate_Move
		{
			move->mRunSpeed += move->mRunAccel;
		}
		else //Normal_Move 
		{
			move->mMoveStateMachine->changeState(Normal_Move::Instance());
			return;
		}
	}

	// move in current body direction (not the goal direction)
	move->mSceneNode->translate(0, 0, move->mCommand.mClientFrametime * move->mRunSpeed,Node::TS_LOCAL);
}
void Accelerate_Move::exit(Move* move)
{
}

Decelerate_Move* Decelerate_Move::Instance()
{
	static Decelerate_Move instance;
	return &instance;
}
void Decelerate_Move::enter(Move* move)
{
}
void Decelerate_Move::execute(Move* move)
{
    if (move->mKeyDirection.isZeroLength()) 
	{
		if(move->mRunSpeed > 0.0) //Decelerate_Move
		{
			move->mRunSpeed -= move->mRunDecel;
		}
        else //No_Move
		{
			move->mMoveStateMachine->changeState(No_Move::Instance());
			return;
		}
    }
	else 
	{
        if(move->mRunSpeed < MAX_RUN_SPEED) //Accelerate_Move
		{
			move->mMoveStateMachine->changeState(Accelerate_Move::Instance());
			return;
		}
		else //Normal_Move 
		{
			move->mMoveStateMachine->changeState(Normal_Move::Instance());
			return;
		}
	}

	// move in current body direction (not the goal direction)
	move->mSceneNode->translate(0, 0, move->mCommand.mClientFrametime * move->mRunSpeed,Node::TS_LOCAL);	
}
void Decelerate_Move::exit(Move* move)
{
}

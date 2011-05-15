#include "rotationStates.h"
#include "rotationStateMachine.h"

#include "rotation.h"

//player,client,shape combo
#include "../player/clientSidePlayer.h"
#include "../client/clientSideClient.h"
#include "../shape/ogreShape.h"

//only using for logstring this needs to be remedied.
#include "../tdreamsock/dreamSock.h"

Normal_Rotation* Normal_Rotation::Instance()
{
  static Normal_Rotation instance;
  return &instance;
}
void Normal_Rotation::enter(Rotation* rotation)
{
	//LogString("STATE: Normal_Rotation");
}
void Normal_Rotation::execute(Rotation* rotation)
{
	// are we too far off
    if(abs(rotation->mDegreesToServer) > 6.0)
	{
		rotation->mPlayer->mClient->mCommand.mCatchupRot = true;
		rotation->mRotationStateMachine->changeState(Catchup_Rotation::Instance());
	}
	else
	{
		if (rotation->mServerRotSpeed == 0.0)
		{
			rotation->mPlayer->mClient->mCommand.mRotSpeed = 0.0;
		}
		else
		{
			// if server rot counter-clockwise hardcode server rot to +500
			if(rotation->mServerRotSpeed > 0.0)
			{
				rotation->mPlayer->mClient->mCommand.mRotSpeed = 250.0;
			}
			else //clockwise - set to -500
			{
				rotation->mPlayer->mClient->mCommand.mRotSpeed = -250.0;
			}
		}
	}
}
void Normal_Rotation::exit(Rotation* rotation)
{
}

Catchup_Rotation* Catchup_Rotation::Instance()
{
  static Catchup_Rotation instance;
  return &instance;
}
void Catchup_Rotation::enter(Rotation* rotation)
{
	//LogString("STATE: Catchup_Rotation");
}
void Catchup_Rotation::execute(Rotation* rotation)
{
    // are we back on track
    if(abs(rotation->mDegreesToServer) < 4.0)
	{
		rotation->mPlayer->mClient->mCommand.mCatchupRot = false;
		rotation->mRotationStateMachine->changeState(Normal_Rotation::Instance());
	}
	else
	{

		//if(mClient->command.mCatchupRot == true && mClient->command.mStop == false)
		if(rotation->mServerRotSpeed != 0.0)
		{
			// if server rot counter-clockwise hardcode server rot to +500
			if(rotation->mServerRotSpeed > 0.0)
			{
				rotation->mPlayer->mClient->mCommand.mRotSpeed = 250.0;
			}
			else //clockwise - set to -500
			{
				rotation->mPlayer->mClient->mCommand.mRotSpeed = -250.0;
			}

			if(rotation->mDegreesToServer/rotation->mServerRotSpeed > 0.0)
			{
				rotation->mPlayer->mClient->mCommand.mRotSpeed = rotation->mPlayer->mClient->mCommand.mRotSpeed * 1.20f;
			}
			else
			{
				rotation->mPlayer->mClient->mCommand.mRotSpeed = rotation->mPlayer->mClient->mCommand.mRotSpeed * 0.8f;
			}
		}
		else if(rotation->mServerRotSpeed == 0.0)
		{
			LogString("catchup2");
			if (rotation->mDegreesToServer > 0.0)
			{
				rotation->mPlayer->mClient->mCommand.mRotSpeed = 250.0;
			}
			else //clockwise - set to -500
			{
				rotation->mPlayer->mClient->mCommand.mRotSpeed = -250.0;
			}
		}
	}
}
void Catchup_Rotation::exit(Rotation* rotation)
{
}


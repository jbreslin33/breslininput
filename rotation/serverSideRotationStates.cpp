#include "serverSideRotationStates.h"
#include "serverSideRotationStateMachine.h"

#include "serverSideRotation.h"

Normal_Rotation* Normal_Rotation::Instance()
{
  static Normal_Rotation instance;
  return &instance;
}
void Normal_Rotation::enter(ServerSideRotation* rotation)
{
}
void Normal_Rotation::execute(ServerSideRotation* rotation)
{
	/*
	// are we too far off
    if(abs(rotation->mDegreesToServer) > rotation->mRotInterpLimitHigh)
        {
                rotation->mCommand.mCatchupRot = true;
                rotation->mRotationStateMachine->changeState(Catchup_Rotation::Instance());
        }
        else
        {
                if (rotation->mServerRotSpeed == 0.0)
                {
                        rotation->mCommand.mRotSpeed = 0.0;
                }
                else
                {
                        // if server rot counter-clockwise hardcode server rot to +mTurnSpeed
                        if(rotation->mServerRotSpeed > 0.0)
                        {
                                rotation->mCommand.mRotSpeed = rotation->mTurnSpeed;
                        }
                        else //clockwise - set to -mTurnSpeed
                        {
                                rotation->mCommand.mRotSpeed = -rotation->mTurnSpeed;
                        }
                }
        }
		*/
}
void Normal_Rotation::exit(ServerSideRotation* rotation)
{
}

Catchup_Rotation* Catchup_Rotation::Instance()
{
  static Catchup_Rotation instance;
  return &instance;
}
void Catchup_Rotation::enter(ServerSideRotation* rotation)
{
}
void Catchup_Rotation::execute(ServerSideRotation* rotation)
{
	/*
	// are we back on track
    if(abs(rotation->mDegreesToServer) < rotation->mRotInterpLimitLow)
    {
		rotation->mCommand.mCatchupRot = false;
        rotation->mRotationStateMachine->changeState(Normal_Rotation::Instance());
    }
    else
    {
		if(rotation->mServerRotSpeed != 0.0)
        {
			// if server rot counter-clockwise hardcode server rot to +mTurnSpeed
            if(rotation->mServerRotSpeed > 0.0)
            {
				rotation->mCommand.mRotSpeed = rotation->mTurnSpeed;
            }
            else //clockwise - set to -mTurnSpeed
            {
				rotation->mCommand.mRotSpeed = -rotation->mTurnSpeed;
            }
			if(rotation->mDegreesToServer/rotation->mServerRotSpeed > 0.0)
            {
				rotation->mCommand.mRotSpeed = rotation->mCommand.mRotSpeed * rotation->mRotInterpIncrease;
            }
            else
            {
				rotation->mCommand.mRotSpeed = rotation->mCommand.mRotSpeed * rotation->mRotInterpDecrease;
            }
		}
        else if(rotation->mServerRotSpeed == 0.0)
        {
			if (rotation->mDegreesToServer > 0.0)
            {
				rotation->mCommand.mRotSpeed = rotation->mTurnSpeed;
            }
            else //clockwise - set to -mTurnSpeed
            {
				rotation->mCommand.mRotSpeed = -rotation->mTurnSpeed;
            }
		}
	}
	*/
}
void Catchup_Rotation::exit(ServerSideRotation* rotation)
{
}


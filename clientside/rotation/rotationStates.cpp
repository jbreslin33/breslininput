#include "rotationStates.h"
#include "rotationStateMachine.h"

#include "rotation.h"

#include "../../billboard/objectTitle.h"

Normal_ProcessTick_Rotation* Normal_ProcessTick_Rotation::Instance()
{
  static Normal_ProcessTick_Rotation instance;
  return &instance;
}
void Normal_ProcessTick_Rotation::enter(Rotation* rotation)
{
}
void Normal_ProcessTick_Rotation::execute(Rotation* rotation)
{
	rotation->mObjectTitleString.append("R:Normal");
	
	// are we too far off
    if(abs(rotation->mDegreesToServer) > rotation->mRotInterpLimitHigh)
    {
        rotation->mRotationProcessTickStateMachine->changeState(Catchup_ProcessTick_Rotation::Instance());
		return;
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
}
void Normal_ProcessTick_Rotation::exit(Rotation* rotation)
{
}

Catchup_ProcessTick_Rotation* Catchup_ProcessTick_Rotation::Instance()
{
  static Catchup_ProcessTick_Rotation instance;
  return &instance;
}
void Catchup_ProcessTick_Rotation::enter(Rotation* rotation)
{
}
void Catchup_ProcessTick_Rotation::execute(Rotation* rotation)
{
	rotation->mObjectTitleString.append("R:Catchup");

	// are we back on track
    if(abs(rotation->mDegreesToServer) < rotation->mRotInterpLimitLow)
    {
        rotation->mRotationProcessTickStateMachine->changeState(Normal_ProcessTick_Rotation::Instance());
		return;
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
}
void Catchup_ProcessTick_Rotation::exit(Rotation* rotation)
{
}

/****************************InterpolateTick************************/

Normal_InterpolateTick_Rotation* Normal_InterpolateTick_Rotation::Instance()
{
  static Normal_InterpolateTick_Rotation instance;
  return &instance;
}
void Normal_InterpolateTick_Rotation::enter(Rotation* rotation)
{
}
void Normal_InterpolateTick_Rotation::execute(Rotation* rotation)
{
	
}
void Normal_InterpolateTick_Rotation::exit(Rotation* rotation)
{
}

Catchup_InterpolateTick_Rotation* Catchup_InterpolateTick_Rotation::Instance()
{
  static Catchup_InterpolateTick_Rotation instance;
  return &instance;
}
void Catchup_InterpolateTick_Rotation::enter(Rotation* rotation)
{
}
void Catchup_InterpolateTick_Rotation::execute(Rotation* rotation)
{
	
}
void Catchup_InterpolateTick_Rotation::exit(Rotation* rotation)
{
}


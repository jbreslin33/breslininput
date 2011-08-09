#include "dynamicShapeRotationStates.h"
#include "dynamicShapeStateMachine.h"

#include "../shape/dynamicShape.h"

#include "../billboard/objectTitle.h"

/******************** Global_ProcessTick_Rotation *****************/
Global_ProcessTick_Rotation* Global_ProcessTick_Rotation::Instance()
{
  static Global_ProcessTick_Rotation instance;
  return &instance;
}
void Global_ProcessTick_Rotation::enter(DynamicShape* dynamicShape)
{
}
void Global_ProcessTick_Rotation::execute(DynamicShape* dynamicShape)
{
	dynamicShape->calculateServerRotationSpeed();

	//shape_replace
	dynamicShape->mGhost->yaw(dynamicShape->mServerRotSpeed,true);	

}
void Global_ProcessTick_Rotation::exit(DynamicShape* dynamicShape)
{
}

/******************** Normal_ProcessTick_Rotation *****************/

Normal_ProcessTick_Rotation* Normal_ProcessTick_Rotation::Instance()
{
  static Normal_ProcessTick_Rotation instance;
  return &instance;
}
void Normal_ProcessTick_Rotation::enter(DynamicShape* dynamicShape)
{
}
void Normal_ProcessTick_Rotation::execute(DynamicShape* dynamicShape)
{
	//->mObjectTitleString.append("R:Normal");
	
	// are we too far off you need to change to catchup state
    if(abs(dynamicShape->mDegreesToServer) > dynamicShape->mRotInterpLimitHigh)
    {
       dynamicShape->mRotationProcessTickStateMachine->changeState(Catchup_ProcessTick_Rotation::Instance());
		return;
    }
    else
    {
         if (dynamicShape->mServerRotSpeed == 0.0)
         {
			dynamicShape->mCommandToRunOnShape.mRotSpeed = 0.0;
         }
         else
         {
			// if server rot counter-clockwise hardcode server rot to +mTurnSpeed
            if(dynamicShape->mServerRotSpeed > 0.0)
            {
				dynamicShape->mCommandToRunOnShape.mRotSpeed =dynamicShape->mTurnSpeed;
            }
			else //clockwise - set to -mTurnSpeed
            {
				dynamicShape->mCommandToRunOnShape.mRotSpeed = -dynamicShape->mTurnSpeed;
            }
		}
	}
}
void Normal_ProcessTick_Rotation::exit(DynamicShape* dynamicShape)
{
}

/******************** Catchup_ProcessTick_Rotation *****************/

Catchup_ProcessTick_Rotation* Catchup_ProcessTick_Rotation::Instance()
{
  static Catchup_ProcessTick_Rotation instance;
  return &instance;
}
void Catchup_ProcessTick_Rotation::enter(DynamicShape* dynamicShape)
{
}
void Catchup_ProcessTick_Rotation::execute(DynamicShape* dynamicShape)
{
	//->mObjectTitleString.append("R:Catchup");

	// are we back on track
    if(abs(dynamicShape->mDegreesToServer) < dynamicShape->mRotInterpLimitLow)
    {
       dynamicShape->mRotationProcessTickStateMachine->changeState(Normal_ProcessTick_Rotation::Instance());
		return;
    }
    else
    {
		if(dynamicShape->mServerRotSpeed != 0.0)
        {
			// if server rot counter-clockwise hardcode server rot to +mTurnSpeed
            if(dynamicShape->mServerRotSpeed > 0.0)
            {
				dynamicShape->mCommandToRunOnShape.mRotSpeed = dynamicShape->mTurnSpeed;
            }
            else //clockwise - set to -mTurnSpeed
            {
				dynamicShape->mCommandToRunOnShape.mRotSpeed = -dynamicShape->mTurnSpeed;
            }
			if(dynamicShape->mDegreesToServer / dynamicShape->mServerRotSpeed > 0.0)
            {
				dynamicShape->mCommandToRunOnShape.mRotSpeed = dynamicShape->mCommandToRunOnShape.mRotSpeed * dynamicShape->mRotInterpIncrease;
            }
            else
            {
				dynamicShape->mCommandToRunOnShape.mRotSpeed = dynamicShape->mCommandToRunOnShape.mRotSpeed * dynamicShape->mRotInterpDecrease;
            }
		}
        else if(dynamicShape->mServerRotSpeed == 0.0)
        {
			if (dynamicShape->mDegreesToServer > 0.0)
            {
				dynamicShape->mCommandToRunOnShape.mRotSpeed = dynamicShape->mTurnSpeed;
            }
            else //clockwise - set to -mTurnSpeed
            {
				dynamicShape->mCommandToRunOnShape.mRotSpeed = -dynamicShape->mTurnSpeed;
            }
		}
	}
}
void Catchup_ProcessTick_Rotation::exit(DynamicShape* dynamicShape)
{
}

/******************** Global_InterpolateTick_Rotation *****************/


Global_InterpolateTick_Rotation* Global_InterpolateTick_Rotation::Instance()
{
  static Global_InterpolateTick_Rotation instance;
  return &instance;
}
void Global_InterpolateTick_Rotation::enter(DynamicShape* dynamicShape)
{
}
void Global_InterpolateTick_Rotation::execute(DynamicShape* dynamicShape)
{
	
}
void Global_InterpolateTick_Rotation::exit(DynamicShape* dynamicShape)
{
}

/******************** Normal_InterpolateTick_Rotation *****************/

Normal_InterpolateTick_Rotation* Normal_InterpolateTick_Rotation::Instance()
{
  static Normal_InterpolateTick_Rotation instance;
  return &instance;
}
void Normal_InterpolateTick_Rotation::enter(DynamicShape* dynamicShape)
{
}
void Normal_InterpolateTick_Rotation::execute(DynamicShape* dynamicShape)
{

	//->mObjectTitleString.append("R:Normal");
	float rotSpeed = dynamicShape->mCommandToRunOnShape.mRotSpeed * dynamicShape->mRenderTime;
   dynamicShape->yaw(rotSpeed, true);

    if (dynamicShape->mServerRotSpeed == 0.0 && abs(dynamicShape->getDegreesToServer()) < dynamicShape->mRotInterpLimitLow)
    {
		dynamicShape->mRotationInterpolateTickStateMachine->changeState(Off_InterpolateTick_Rotation::Instance());
    }

}
void Normal_InterpolateTick_Rotation::exit(DynamicShape* dynamicShape)
{
}

/******************** Off_InterpolateTick_Rotation *****************/

Off_InterpolateTick_Rotation* Off_InterpolateTick_Rotation::Instance()
{
  static Off_InterpolateTick_Rotation instance;
  return &instance;
}
void Off_InterpolateTick_Rotation::enter(DynamicShape* dynamicShape)
{
	dynamicShape->mCommandToRunOnShape.mRotSpeed = 0.0;
}
void Off_InterpolateTick_Rotation::execute(DynamicShape* dynamicShape)
{		
	//->mObjectTitleString.append("R:Off");
	if (abs(dynamicShape->getDegreesToServer()) > dynamicShape->mRotInterpLimitLow)
    {
		dynamicShape->mRotationInterpolateTickStateMachine->changeState(Normal_InterpolateTick_Rotation::Instance());
    }
}
void Off_InterpolateTick_Rotation::exit(DynamicShape* dynamicShape)
{
}


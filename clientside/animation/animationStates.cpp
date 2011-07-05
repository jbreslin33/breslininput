#include "animationStates.h"
#include "animationStateMachine.h"

#include "../../clientside/shape/shape.h"

#include "animation.h"

#include "../../billboard/objectTitle.h"


/******************** Global_ProcessTick_Animation *****************/
Global_ProcessTick_Animation* Global_ProcessTick_Animation::Instance()
{
  static Global_ProcessTick_Animation instance;
  return &instance;
}
void Global_ProcessTick_Animation::enter(Animation* animation)
{
}
void Global_ProcessTick_Animation::execute(Animation* animation)
{
	animation->calculateServerAnimationSpeed();

	// yaw server guy to new rot
	//animation->mGame->mShapeGhostVector.at
    animation->mGhost->getSceneNode()->yaw(Degree(animation->mServerRotSpeed));	
}
void Global_ProcessTick_Animation::exit(Animation* animation)
{
}

/******************** Normal_ProcessTick_Animation *****************/

Normal_ProcessTick_Animation* Normal_ProcessTick_Animation::Instance()
{
  static Normal_ProcessTick_Animation instance;
  return &instance;
}
void Normal_ProcessTick_Animation::enter(Animation* animation)
{
}
void Normal_ProcessTick_Animation::execute(Animation* animation)
{
	animation->mObjectTitleString.append("R:Normal");
	
	// are we too far off you need to change to catchup state
    if(abs(animation->mDegreesToServer) > animation->mRotInterpLimitHigh)
    {
        animation->mAnimationProcessTickStateMachine->changeState(Catchup_ProcessTick_Animation::Instance());
		return;
    }
    else
    {
         if (animation->mServerRotSpeed == 0.0)
         {
			animation->mCommandToRunOnShape.mRotSpeed = 0.0;
         }
         else
         {
			// if server rot counter-clockwise hardcode server rot to +mTurnSpeed
            if(animation->mServerRotSpeed > 0.0)
            {
				animation->mCommandToRunOnShape.mRotSpeed = animation->mTurnSpeed;
            }
			else //clockwise - set to -mTurnSpeed
            {
				animation->mCommandToRunOnShape.mRotSpeed = -animation->mTurnSpeed;
            }
		}
	}
}
void Normal_ProcessTick_Animation::exit(Animation* animation)
{
}

/******************** Catchup_ProcessTick_Animation *****************/

Catchup_ProcessTick_Animation* Catchup_ProcessTick_Animation::Instance()
{
  static Catchup_ProcessTick_Animation instance;
  return &instance;
}
void Catchup_ProcessTick_Animation::enter(Animation* animation)
{
}
void Catchup_ProcessTick_Animation::execute(Animation* animation)
{
	animation->mObjectTitleString.append("R:Catchup");

	// are we back on track
    if(abs(animation->mDegreesToServer) < animation->mRotInterpLimitLow)
    {
        animation->mAnimationProcessTickStateMachine->changeState(Normal_ProcessTick_Animation::Instance());
		return;
    }
    else
    {
		if(animation->mServerRotSpeed != 0.0)
        {
			// if server rot counter-clockwise hardcode server rot to +mTurnSpeed
            if(animation->mServerRotSpeed > 0.0)
            {
				animation->mCommandToRunOnShape.mRotSpeed = animation->mTurnSpeed;
            }
            else //clockwise - set to -mTurnSpeed
            {
				animation->mCommandToRunOnShape.mRotSpeed = -animation->mTurnSpeed;
            }
			if(animation->mDegreesToServer/animation->mServerRotSpeed > 0.0)
            {
				animation->mCommandToRunOnShape.mRotSpeed = animation->mCommandToRunOnShape.mRotSpeed * animation->mRotInterpIncrease;
            }
            else
            {
				animation->mCommandToRunOnShape.mRotSpeed = animation->mCommandToRunOnShape.mRotSpeed * animation->mRotInterpDecrease;
            }
		}
        else if(animation->mServerRotSpeed == 0.0)
        {
			if (animation->mDegreesToServer > 0.0)
            {
				animation->mCommandToRunOnShape.mRotSpeed = animation->mTurnSpeed;
            }
            else //clockwise - set to -mTurnSpeed
            {
				animation->mCommandToRunOnShape.mRotSpeed = -animation->mTurnSpeed;
            }
		}
	}
}
void Catchup_ProcessTick_Animation::exit(Animation* animation)
{
}

/******************** Global_InterpolateTick_Animation *****************/


Global_InterpolateTick_Animation* Global_InterpolateTick_Animation::Instance()
{
  static Global_InterpolateTick_Animation instance;
  return &instance;
}
void Global_InterpolateTick_Animation::enter(Animation* animation)
{
}
void Global_InterpolateTick_Animation::execute(Animation* animation)
{
	
}
void Global_InterpolateTick_Animation::exit(Animation* animation)
{
}

/******************** Normal_InterpolateTick_Animation *****************/

Normal_InterpolateTick_Animation* Normal_InterpolateTick_Animation::Instance()
{
  static Normal_InterpolateTick_Animation instance;
  return &instance;
}
void Normal_InterpolateTick_Animation::enter(Animation* animation)
{
}
void Normal_InterpolateTick_Animation::execute(Animation* animation)
{

	//animation->mObjectTitleString.append("R:Normal");
	float rotSpeed = animation->mCommandToRunOnShape.mRotSpeed * animation->mRenderTime;
    animation->getSceneNode()->yaw(Degree(rotSpeed));

    if (animation->mServerRotSpeed == 0.0 && abs(animation->getDegreesToServer()) < animation->mRotInterpLimitLow)
    {
		animation->mAnimationInterpolateTickStateMachine->changeState(Off_InterpolateTick_Animation::Instance());
    }

}
void Normal_InterpolateTick_Animation::exit(Animation* animation)
{
}

/******************** Off_InterpolateTick_Animation *****************/

Off_InterpolateTick_Animation* Off_InterpolateTick_Animation::Instance()
{
  static Off_InterpolateTick_Animation instance;
  return &instance;
}
void Off_InterpolateTick_Animation::enter(Animation* animation)
{
	animation->mCommandToRunOnShape.mRotSpeed = 0.0;
}
void Off_InterpolateTick_Animation::execute(Animation* animation)
{		
	//animation->mObjectTitleString.append("R:Off");
	if (abs(animation->getDegreesToServer()) > animation->mRotInterpLimitLow)
    {
		animation->mAnimationInterpolateTickStateMachine->changeState(Normal_InterpolateTick_Animation::Instance());
    }
}
void Off_InterpolateTick_Animation::exit(Animation* animation)
{
}


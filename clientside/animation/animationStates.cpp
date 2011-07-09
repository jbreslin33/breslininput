#include "animationStates.h"
#include "animationStateMachine.h"

#include "../../tdreamsock/dreamSockLog.h"

#include "../../clientside/shape/shape.h"

#include "animation.h"

#include "../../billboard/objectTitle.h"


/******************** Global_ProcessTick_Animation *****************/
Global_ProcessTick_Animation* Global_ProcessTick_Animation::Instance()
{
  static Global_ProcessTick_Animation instance;
  return &instance;
}
void Global_ProcessTick_Animation::enter(OgreAnimation* animation)
{
}
void Global_ProcessTick_Animation::execute(OgreAnimation* animation)
{

}
void Global_ProcessTick_Animation::exit(OgreAnimation* animation)
{
}

/******************** Idle_ProcessTick_Animation *****************/

Idle_ProcessTick_Animation* Idle_ProcessTick_Animation::Instance()
{
  static Idle_ProcessTick_Animation instance;
  return &instance;
}
void Idle_ProcessTick_Animation::enter(OgreAnimation* animation)
{
}
void Idle_ProcessTick_Animation::execute(OgreAnimation* animation)
{
}
void Idle_ProcessTick_Animation::exit(OgreAnimation* animation)
{
}

/******************** Run_ProcessTick_Animation *****************/

Run_ProcessTick_Animation* Run_ProcessTick_Animation::Instance()
{
  static Run_ProcessTick_Animation instance;
  return &instance;
}
void Run_ProcessTick_Animation::enter(OgreAnimation* animation)
{
}
void Run_ProcessTick_Animation::execute(OgreAnimation* animation)
{
}
void Run_ProcessTick_Animation::exit(OgreAnimation* animation)
{
}

/******************** Global_InterpolateTick_Animation *****************/


Global_InterpolateTick_Animation* Global_InterpolateTick_Animation::Instance()
{
  static Global_InterpolateTick_Animation instance;
  return &instance;
}
void Global_InterpolateTick_Animation::enter(OgreAnimation* animation)
{
}
void Global_InterpolateTick_Animation::execute(OgreAnimation* animation)
{
	
}
void Global_InterpolateTick_Animation::exit(OgreAnimation* animation)
{
}

/******************** Idle_InterpolateTick_Animation *****************/

Idle_InterpolateTick_Animation* Idle_InterpolateTick_Animation::Instance()
{
  static Idle_InterpolateTick_Animation instance;
  return &instance;
}
void Idle_InterpolateTick_Animation::enter(OgreAnimation* animation)
{
	// start off in the idle state (top and bottom together)
	animation->setBaseAnimation(ANIM_IDLE_BASE,false);
	animation->setTopAnimation(ANIM_IDLE_TOP,false);

	// relax the hands since we're not holding anything
	animation->mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
}
void Idle_InterpolateTick_Animation::execute(OgreAnimation* animation)
{
	animation->mObjectTitleString.append(" A:IDLE");
	LogString("here i am");
	if (!animation->mCommandToRunOnShape.mVelocity.isZero())
	{
		animation->mAnimationInterpolateTickStateMachine->changeState(Run_InterpolateTick_Animation::Instance());
	}
}
void Idle_InterpolateTick_Animation::exit(OgreAnimation* animation)
{
}

/******************** Run_InterpolateTick_Animation *****************/

Run_InterpolateTick_Animation* Run_InterpolateTick_Animation::Instance()
{
  static Run_InterpolateTick_Animation instance;
  return &instance;
}
void Run_InterpolateTick_Animation::enter(OgreAnimation* animation)
{
	animation->setBaseAnimation(ANIM_RUN_BASE, true);
	animation->setTopAnimation(ANIM_RUN_TOP, true);

	// relax the hands since we're not holding anything
	animation->mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);

}
void Run_InterpolateTick_Animation::execute(OgreAnimation* animation)
{
	animation->mObjectTitleString.append(" A:IDLE");

	if (animation->mCommandToRunOnShape.mVelocity.isZero())
	{
		animation->mAnimationInterpolateTickStateMachine->changeState(Idle_InterpolateTick_Animation::Instance());
	}
}
void Run_InterpolateTick_Animation::exit(OgreAnimation* animation)
{
}

/******************** Off_InterpolateTick_Animation *****************/

Off_InterpolateTick_Animation* Off_InterpolateTick_Animation::Instance()
{
  static Off_InterpolateTick_Animation instance;
  return &instance;
}
void Off_InterpolateTick_Animation::enter(OgreAnimation* animation)
{
}
void Off_InterpolateTick_Animation::execute(OgreAnimation* animation)
{		

}
void Off_InterpolateTick_Animation::exit(OgreAnimation* animation)
{
}


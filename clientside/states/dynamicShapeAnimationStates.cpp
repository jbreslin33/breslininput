#include "dynamicShapeAnimationStates.h"
#include "dynamicShapeStateMachine.h"

#include "../shape/dynamicShape.h"

#include "../billboard/objectTitle.h"


/******************** Global_InterpolateTick_Animation *****************/
Global_InterpolateTick_Animation* Global_InterpolateTick_Animation::Instance()
{
  static Global_InterpolateTick_Animation instance;
  return &instance;
}
void Global_InterpolateTick_Animation::enter(DynamicShape* dynamicShape)
{
}
void Global_InterpolateTick_Animation::execute(DynamicShape* dynamicShape)
{

}
void Global_InterpolateTick_Animation::exit(DynamicShape* dynamicShape)
{
}

/******************** Idle_InterpolateTick_Animation *****************/

Idle_InterpolateTick_Animation* Idle_InterpolateTick_Animation::Instance()
{
  static Idle_InterpolateTick_Animation instance;
  return &instance;
}
void Idle_InterpolateTick_Animation::enter(DynamicShape* dynamicShape)
{
	dynamicShape->enterAnimationState(this);
}

void Idle_InterpolateTick_Animation::execute(DynamicShape* dynamicShape)
{
	if (!dynamicShape->mCommandToRunOnShape.mVelocity.isZero())
	{
		dynamicShape->mAnimationInterpolateTickStateMachine->changeState(Run_InterpolateTick_Animation::Instance());
	}

	dynamicShape->runAnimations();
}
void Idle_InterpolateTick_Animation::exit(DynamicShape* dynamicShape)
{
}

/******************** Run_InterpolateTick_Animation *****************/

Run_InterpolateTick_Animation* Run_InterpolateTick_Animation::Instance()
{
  static Run_InterpolateTick_Animation instance;
  return &instance;
}
void Run_InterpolateTick_Animation::enter(DynamicShape* dynamicShape)
{
	dynamicShape->enterAnimationState(this);
}
void Run_InterpolateTick_Animation::execute(DynamicShape* dynamicShape)
{
	if (dynamicShape->mCommandToRunOnShape.mVelocity.isZero())
	{
		dynamicShape->mAnimationInterpolateTickStateMachine->changeState(Idle_InterpolateTick_Animation::Instance());
	}
	
	dynamicShape->runAnimations();
}

void Run_InterpolateTick_Animation::exit(DynamicShape* dynamicShape)
{
}



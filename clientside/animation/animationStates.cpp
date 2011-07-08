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
void Global_ProcessTick_Animation::enter(OgreAnimation* animation)
{
}
void Global_ProcessTick_Animation::execute(OgreAnimation* animation)
{

}
void Global_ProcessTick_Animation::exit(OgreAnimation* animation)
{
}

/******************** Normal_ProcessTick_Animation *****************/

Normal_ProcessTick_Animation* Normal_ProcessTick_Animation::Instance()
{
  static Normal_ProcessTick_Animation instance;
  return &instance;
}
void Normal_ProcessTick_Animation::enter(OgreAnimation* animation)
{
}
void Normal_ProcessTick_Animation::execute(OgreAnimation* animation)
{
	animation->mObjectTitleString.append("A:Normal");
	
	
}
void Normal_ProcessTick_Animation::exit(OgreAnimation* animation)
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

/******************** Normal_InterpolateTick_Animation *****************/

Normal_InterpolateTick_Animation* Normal_InterpolateTick_Animation::Instance()
{
  static Normal_InterpolateTick_Animation instance;
  return &instance;
}
void Normal_InterpolateTick_Animation::enter(OgreAnimation* animation)
{
}
void Normal_InterpolateTick_Animation::execute(OgreAnimation* animation)
{

	

}
void Normal_InterpolateTick_Animation::exit(OgreAnimation* animation)
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
	animation->mCommandToRunOnShape.mRotSpeed = 0.0;
}
void Off_InterpolateTick_Animation::execute(OgreAnimation* animation)
{		

}
void Off_InterpolateTick_Animation::exit(OgreAnimation* animation)
{
}


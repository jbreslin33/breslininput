#include "rotationStates.h"
#include "rotationStateMachine.h"

#include "../../clientside/shape/shape.h"

#include "rotation.h"

#include "../../billboard/objectTitle.h"



Global_ProcessTick_Rotation* Global_ProcessTick_Rotation::Instance()
{
  static Global_ProcessTick_Rotation instance;
  return &instance;
}
void Global_ProcessTick_Rotation::enter(Rotation* rotation)
{
}
void Global_ProcessTick_Rotation::execute(Rotation* rotation)
{
    rotation->mServerRotOld  = Ogre::Vector3::ZERO;
    rotation->mServerRotNew  = Ogre::Vector3::ZERO;

    rotation->mServerRotOld.x = rotation->mServerFrame.mRotOld.x;
    rotation->mServerRotOld.z = rotation->mServerFrame.mRotOld.z;

    rotation->mServerRotNew.x = rotation->mServerFrame.mRot.x;
    rotation->mServerRotNew.z = rotation->mServerFrame.mRot.z;

    rotation->mServerRotNew.normalise();
    rotation->mServerRotOld.normalise();

    //calculate how far off we are from server
    Quaternion toServer = rotation->getSceneNode()->getOrientation().zAxis().getRotationTo(rotation->mServerRotNew,Vector3::UNIT_Y);

    // convert to degrees
    rotation->mDegreesToServer = toServer.getYaw().valueDegrees();

    //calculate server rotation from last tick to new one
    Quaternion serverRot = rotation->mServerShape->getSceneNode()->getOrientation().zAxis().getRotationTo(rotation->mServerRotNew, Vector3::UNIT_Y);

    // convert to degrees
    rotation->mServerRotSpeed = serverRot.getYaw().valueDegrees();

    if(abs(rotation->mServerRotSpeed) < 0.01)
    {
		rotation->mServerRotSpeed = 0.0;
    }
    
	// yaw server guy to new rot
    rotation->mServerShape->getSceneNode()->yaw(Degree(rotation->mServerRotSpeed));	
}
void Global_ProcessTick_Rotation::exit(Rotation* rotation)
{
}


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


Global_InterpolateTick_Rotation* Global_InterpolateTick_Rotation::Instance()
{
  static Global_InterpolateTick_Rotation instance;
  return &instance;
}
void Global_InterpolateTick_Rotation::enter(Rotation* rotation)
{
}
void Global_InterpolateTick_Rotation::execute(Rotation* rotation)
{
	
}
void Global_InterpolateTick_Rotation::exit(Rotation* rotation)
{
}

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
	float rotSpeed = rotation->mCommand.mRotSpeed * rotation->mRenderTime;
    rotation->getSceneNode()->yaw(Degree(rotSpeed));

    if (rotation->mServerRotSpeed == 0.0 && abs(rotation->getDegreesToServer()) < rotation->mRotInterpLimitLow)
    {
		rotation->mRotationInterpolateTickStateMachine->changeState(Off_InterpolateTick_Rotation::Instance());
    }

}
void Normal_InterpolateTick_Rotation::exit(Rotation* rotation)
{
}

Off_InterpolateTick_Rotation* Off_InterpolateTick_Rotation::Instance()
{
  static Off_InterpolateTick_Rotation instance;
  return &instance;
}
void Off_InterpolateTick_Rotation::enter(Rotation* rotation)
{
	rotation->mCommand.mRotSpeed = 0.0;
}
void Off_InterpolateTick_Rotation::execute(Rotation* rotation)
{		
	if (abs(rotation->getDegreesToServer()) > rotation->mRotInterpLimitLow)
    {
		rotation->mRotationInterpolateTickStateMachine->changeState(Normal_InterpolateTick_Rotation::Instance());
    }
}
void Off_InterpolateTick_Rotation::exit(Rotation* rotation)
{
}


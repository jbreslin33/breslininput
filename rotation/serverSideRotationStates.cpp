#include "serverSideRotationStates.h"
#include "serverSideRotationStateMachine.h"

#include "serverSideRotation.h"

#define TURN_SPEED 250.0f      // character turning in degrees per second

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
	float clientFrametime;

    clientFrametime = rotation->mCommand.mMilliseconds / 1000.0f;;

	rotation->mGoalDirection = Vector3::ZERO;   // we will calculate this

	Real yawAtSpeed;

	rotation->mGoalDirection += rotation->mKeyDirection.z * Vector3::UNIT_Z;
    rotation->mGoalDirection += rotation->mKeyDirection.x * Vector3::UNIT_X;
    rotation->mGoalDirection.y = 0;
    rotation->mGoalDirection.normalise();

    Quaternion toGoal = rotation->mSceneNode->getOrientation().zAxis().getRotationTo
		(rotation->mGoalDirection,Vector3::UNIT_Y);
    
	// calculate how much the character has to turn to face goal direction
    Real yawToGoal = toGoal.getYaw().valueDegrees();

    // this is how much the character CAN turn this frame
    if(yawToGoal == 0.0)
	{
		yawAtSpeed = 0.0;
	}
    else
	{
		yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * clientFrametime * TURN_SPEED;
	}

    // turn as much as we can, but not more than we need to
    if (yawToGoal < 0)
	{
		yawToGoal = std::min<Real>(0, std::max<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, yawAtSpeed, 0);
	}         
	else if (yawToGoal > 0)
	{
		yawToGoal = std::max<Real>(0, std::min<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, 0, yawAtSpeed);
	}               
    rotation->mSceneNode->yaw(Degree(yawToGoal));
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
	
}
void Catchup_Rotation::exit(ServerSideRotation* rotation)
{
}


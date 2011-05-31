#include "serverSideRotation.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../shape/serverSideShape.h"

#include <string>

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//rotation states
#include "serverSideRotationStateMachine.h"
#include "serverSideRotationStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16

#define TURN_SPEED 250.0f      // character turning in degrees per second

ServerSideRotation::ServerSideRotation(std::string name, Vector3D* position,
									   Ogre::Root* root)
:
	OgreShape(name,position,root),
	Rotation (                  )
{
	//keys
    mKeyDirection = Vector3::ZERO;
	mGoalDirection = Vector3::ZERO;

	//rotation states
	mRotationStateMachine = new ServerSideRotationStateMachine(this);    //setup the state machine
	mRotationStateMachine->setCurrentState      (Normal_Rotation::Instance());
	mRotationStateMachine->setPreviousState     (Normal_Rotation::Instance());
	mRotationStateMachine->setGlobalState       (NULL);
}

ServerSideRotation::~ServerSideRotation()
{
}

void ServerSideRotation::processTick()
{
	float clientFrametime;

    clientFrametime = mCommand.mMilliseconds / 1000.0f;;


	mGoalDirection = Vector3::ZERO;   // we will calculate this

Real yawAtSpeed;

		mGoalDirection += mKeyDirection.z * Vector3::UNIT_Z;
        mGoalDirection += mKeyDirection.x * Vector3::UNIT_X;
        mGoalDirection.y = 0;
        mGoalDirection.normalise();

        Quaternion toGoal = mSceneNode->getOrientation().zAxis().getRotationTo(mGoalDirection,Vector3::UNIT_Y);
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
        mSceneNode->yaw(Degree(yawToGoal));
}



void ServerSideRotation::setKeyDirection()
{
	mKeyDirection.x = 0;
    mKeyDirection.z = 0;
    mKeyDirection.y = 0;
                
    // keep track of the player's intended direction
    if(mCommand.mKey & KEY_UP) 
	{
		mKeyDirection.z += -1;
	}
    if(mCommand.mKey & KEY_LEFT) 
	{
		mKeyDirection.x += -1;
	}
                
    if(mCommand.mKey & KEY_DOWN) 
	{
        mKeyDirection.z += 1;
	}
    if(mCommand.mKey & KEY_RIGHT) 
	{
		mKeyDirection.x += 1;
	}

}
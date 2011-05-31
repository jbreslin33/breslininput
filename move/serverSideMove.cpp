#include "serverSideMove.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../client/client.h"

#define MAX_RUN_SPEED 200           // character running speed in units per second
#define TURN_SPEED 250.0f      // character turning in degrees per second

#include <string>

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//move states
#include "serverSideMoveStateMachine.h"
#include "serverSideMoveStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16

ServerSideMove::ServerSideMove(std::string name, Vector3D* position, Ogre::Root* root)
:
	OgreShape(name,position,root),
	Move     (                  ) 
{
	
	//keys
    mKeyDirection = Vector3::ZERO;
	mGoalDirection = Vector3::ZERO;

	//run acceleration
    mRunAccel    = 5.0;
    mRunDecel    = 5.0;

	//jumping
    mJumping  = false;
	mVerticalVelocity = 0.0;
    mJumpAccel   = 300.0;
    mGravity     = 900.0;

  	//move states
	mMoveStateMachine = new ServerSideMoveStateMachine(this);    //setup the state machine
	mMoveStateMachine->setCurrentState      (Normal_Move::Instance());
	mMoveStateMachine->setPreviousState     (Normal_Move::Instance());
	mMoveStateMachine->setGlobalState       (NULL);
}

ServerSideMove::~ServerSideMove()
{
}
void ServerSideMove::processTick()
{

	//i can see it looks like i need to do rotation first but only if
	//mkey != 0
	float clientFrametime;

    clientFrametime = mCommand.mMilliseconds / 1000.0f;;

	setKeyDirection();

    mGoalDirection = Vector3::ZERO;   // we will calculate this
    Real yawAtSpeed;

    if (mCommand.mKey == 0)
	{
		if(mRunSpeed > 0.0)
		{
			mRunSpeed -= mRunDecel;
		}
        else
		{
           mRunSpeed = 0.0;
		}
    }
    else
    {
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

	if (mCommand.mKey != 0)
	{
		//this only gets called if their is mKey so should it be the 
		//first thing that gets called on move?
        if(mRunSpeed < MAX_RUN_SPEED)
		{
			mRunSpeed += mRunAccel;
		}
	}

    // move in current body direction (not the goal direction)
    mSceneNode->translate(0, 0, clientFrametime * mRunSpeed,Node::TS_LOCAL);

    if(mJumping)
    {
		//LogString("mJumping");
        mSceneNode->translate(0, clientFrametime * mVerticalVelocity, 0, Node::TS_LOCAL);
        mVerticalVelocity -= mGravity * clientFrametime;

        if(mSceneNode->getPosition().y < 0.0)
        {
			//LogString("if");
            mSceneNode->setPosition(mSceneNode->getPosition().x, 0.0, mSceneNode->getPosition().z);
            mVerticalVelocity = 0.0;
            mJumping = false;
        }
	}
}

void ServerSideMove::setKeyDirection()
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
    if(mCommand.mKey & KEY_SPACE) 
    {
		if(!mJumping)
		{
			startJump();
	    }
    }
}

void ServerSideMove::startJump()
{
	mJumping = true;
	mVerticalVelocity = mJumpAccel;
}

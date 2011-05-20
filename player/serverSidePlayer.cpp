#include "serverSidePlayer.h"
#include "../tdreamsock/dreamSock.h"

#include "../shape/ogreShape.h"

#include "../math/vector3D.h"
#include <math.h>

#define RUN_SPEED 200           // character running speed in units per second
#define TURN_SPEED 250.0f      // character turning in degrees per second

#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8

ServerSidePlayer::ServerSidePlayer(std::string name, dreamClient* client, OgreShape* shape) : Player(name)
{
	mClient = client;
	mShape  = shape;

	mKeyDirection = Vector3::ZERO;
    mGoalDirection = Vector3::ZERO;
}

ServerSidePlayer::~ServerSidePlayer()
{
}

void ServerSidePlayer::processTick()
{
	float clientFrametime;

	float multiplier = 200.0f;

	clientFrametime = mCommand.mMilliseconds / 1000.0f;;

	calculateVelocity(&mCommand, clientFrametime);

	setKeyDirection();

	mGoalDirection = Vector3::ZERO;   // we will calculate this
	Real yawAtSpeed;

	if (!mKeyDirection.isZeroLength())
	{
		mGoalDirection += mKeyDirection.z * Vector3::UNIT_Z;
		mGoalDirection += mKeyDirection.x * Vector3::UNIT_X;


		mGoalDirection.y = 0;
		mGoalDirection.normalise();

		Quaternion toGoal = mShape->mSceneNode->getOrientation().zAxis().getRotationTo(mGoalDirection,Vector3::UNIT_Y);

		// calculate how much the character has to turn to face goal direction
		Real yawToGoal = toGoal.getYaw().valueDegrees();

		// this is how much the character CAN turn this frame
		if(yawToGoal == 0.0)
			yawAtSpeed = 0.0;
		else
		   yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * clientFrametime * TURN_SPEED;
	
		// turn as much as we can, but not more than we need to
		if (yawToGoal < 0) 
			yawToGoal = std::min<Real>(0, std::max<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, yawAtSpeed, 0);
		else if (yawToGoal > 0)
			yawToGoal = std::max<Real>(0, std::min<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, 0, yawAtSpeed);
			
		mShape->mSceneNode->yaw(Degree(yawToGoal));

		// move in current body direction (not the goal direction)
		mShape->mSceneNode->translate(0, 0, clientFrametime * RUN_SPEED,
			Node::TS_LOCAL);

	}

	mCommand.mOrigin.x = mShape->mSceneNode->getPosition().x;
	mCommand.mOrigin.z = mShape->mSceneNode->getPosition().z;

	if(mCommand.mVelocity.x != 0.0 || mCommand.mVelocity.z != 0.0)
	{
	   mCommand.mVelocity.x = mShape->mSceneNode->getOrientation().zAxis().x;
	   mCommand.mVelocity.z = mShape->mSceneNode->getOrientation().zAxis().z;
	}

	mCommand.mRot.x = mShape->mSceneNode->getOrientation().zAxis().x;
	mCommand.mRot.z = mShape->mSceneNode->getOrientation().zAxis().z;

	int f = netClient->GetIncomingSequence() & (COMMAND_HISTORY_SIZE-1);
	mProcessedFrame = f;
}
void ServerSidePlayer::calculateVelocity(Command* command, float frametime)
{

	float multiplier = 200.0f;

	command->mVelocity.x = 0.0f;
	command->mVelocity.z = 0.0f;

	if(command->mKey & KEY_UP)
	{
		command->mVelocity.z += -1;
	}

	if(command->mKey & KEY_DOWN)
	{
        command->mVelocity.z += 1;
	}

	if(command->mKey & KEY_LEFT)
	{
		command->mVelocity.x += -1;
	}

	if(command->mKey & KEY_RIGHT)
	{
		command->mVelocity.x += 1;
	}

	float length = sqrt(pow(command->mVelocity.x, 2) + pow(command->mVelocity.z, 2));
	if(length != 0.0)
	{
	   command->mVelocity.x = command->mVelocity.x/length * 0.2f * frametime * 1000;
	   command->mVelocity.z = command->mVelocity.z/length * 0.2f * frametime * 1000;
	}
}


void ServerSidePlayer::setKeyDirection()
{
	if(mShape) 
	{
		mKeyDirection.x = 0;
		mKeyDirection.z = 0;
		mKeyDirection.y = 0;
		
		// keep track of the player's intended direction
		if(mCommand.mKey & KEY_UP) 
			mKeyDirection.z += -1;

		if(mCommand.mKey & KEY_LEFT) 
			mKeyDirection.x += -1;
		
		if(mCommand.mKey & KEY_DOWN) 
           mKeyDirection.z += 1;

		if(mCommand.mKey & KEY_RIGHT) 
			mKeyDirection.x += 1;
	}
}

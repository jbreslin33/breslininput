#include "clientSidePlayer.h"
#include "../tdreamsock/dreamSock.h"

#include "../client/clientSideClient.h"
#include "../shape/ogreShape.h"

#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
ClientSidePlayer::ClientSidePlayer(std::string name, ClientSideClient* client, OgreShape* shape) : Player(name)
{
	mClient = client;
	mShape  = shape;

}

ClientSidePlayer::~ClientSidePlayer()
{
}

void ClientSidePlayer::processTick()
{

	float deltaX = mClient->mServerFrame.mOrigin.x - mShape->getSceneNode()->getPosition().x;
    float deltaZ = mClient->mServerFrame.mOrigin.z - mShape->getSceneNode()->getPosition().z;

	float dist = sqrt(pow(deltaX, 2) + pow(deltaZ, 2));

	if(dist > 8.0)
	{
		mClient->mCommand.mCatchup = true; 
	}

	if(dist < 2.0)
	{
        mClient->mCommand.mCatchup = false;
	}
	if(mClient->mServerFrame.mVelocity.x == 0.0 && mClient->mServerFrame.mVelocity.z == 0.0)
	{
		mClient->mCommand.mStop = true;
	}
	else
	{
        mClient->mCommand.mStop = false;
	}
	     
	if (mClient->mCommand.mCatchup == true && mClient->mCommand.mStop == false)
	{
		Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO;
		Ogre::Vector3 myDest      = Ogre::Vector3::ZERO;
		   
		serverDest.x = mClient->mServerFrame.mVelocity.x;
		serverDest.z = mClient->mServerFrame.mVelocity.z;
		serverDest.normalise();

		float multiplier = dist * 10.0;
		serverDest = serverDest * multiplier;
		serverDest.x = mClient->mServerFrame.mOrigin.x + serverDest.x;
		serverDest.z = mClient->mServerFrame.mOrigin.z + serverDest.z;

		myDest.x = serverDest.x - mShape->getSceneNode()->getPosition().x;
		myDest.z = serverDest.z - mShape->getSceneNode()->getPosition().z;

		float predictDist = pow(myDest.x, 2) + pow(myDest.z, 2);
		predictDist = sqrt(predictDist);

		myDest.normalise();

		float vel = sqrt(pow(mClient->mServerFrame.mVelocity.x, 2) + pow(mClient->mServerFrame.mVelocity.z, 2))/mClient->mCommand.mMilliseconds;
        float time = dist * 10.0/vel;

		myDest = myDest * predictDist/time;

		mClient->mCommand.mVelocity.x = myDest.x;
	    mClient->mCommand.mVelocity.z = myDest.z;
   }
   else
   {
        Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO;
		Ogre::Vector3 myDest      = Ogre::Vector3::ZERO;
		   
	    serverDest.x = mClient->mServerFrame.mVelocity.x;
		serverDest.z = mClient->mServerFrame.mVelocity.z;
		serverDest.normalise();
        serverDest = serverDest * 0.1;

		mClient->mCommand.mVelocity.x = serverDest.x;
	    mClient->mCommand.mVelocity.z = serverDest.z;
	}
}

void ClientSidePlayer::interpolateTick(float renderTime)
{
	Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

	if(mClient->mCommand.mCatchup == true && mClient->mCommand.mStop == false)
	{
		transVector.x = mClient->mCommand.mVelocity.x;
		transVector.z = mClient->mCommand.mVelocity.z;
		mShape->getSceneNode()->translate(transVector * renderTime * 1000, Ogre::Node::TS_WORLD);
	}
    else
	{
		transVector.x = mClient->mCommand.mVelocity.x;
		transVector.z = mClient->mCommand.mVelocity.z;
		mShape->getSceneNode()->translate(transVector * renderTime * 1000, Ogre::Node::TS_WORLD);
	}

	transVector.normalise();

	Ogre::Quaternion toGoal = mShape->getSceneNode()->getOrientation().zAxis().getRotationTo(transVector);
	Ogre::Real yawToGoal = toGoal.getYaw().valueDegrees();

	// this is how much the character CAN turn this frame
	Real yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * renderTime * TURN_SPEED;

	// turn as much as we can, but not more than we need to
	if (yawToGoal < 0)
	{
		yawToGoal = std::min<Real>(0, std::max<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, yawAtSpeed, 0);
	}
	else if (yawToGoal > 0) 
	{
		yawToGoal = std::max<Real>(0, std::min<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, 0, yawAtSpeed);
	}	

	mShape->getSceneNode()->yaw(Ogre::Degree(yawToGoal));

	mShape->updateAnimations(renderTime);
}

void ClientSidePlayer::calculateVelocity(Command* command, float frametime)
{
	float multiplier = 100.0f;

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
		command->mVelocity.x = command->mVelocity.x/length * 0.1 * frametime * 1000;
		command->mVelocity.z = command->mVelocity.z/length * 0.1 * frametime * 1000;
	}
}

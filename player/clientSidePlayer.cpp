#include "clientSidePlayer.h"
#include "../tdreamsock/dreamSock.h"

#include "../client/clientSideClient.h"
#include "../shape/ogreShape.h"
#include "../animation/ogreAnimation.h"

#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8

ClientSidePlayer::ClientSidePlayer(std::string name, ClientSideClient* client, OgreShape* shape) : Player(name)
{
	mClient = client;
	mShape  = shape;
	serverRotSpeed = 0.0;
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
          mClient->mCommand.mCatchup = false;

	   if(mClient->mServerFrame.mVelocity.x == 0.0 && mClient->mServerFrame.mVelocity.z == 0.0)
	   {
		   mClient->mCommand.mStop = true;
	   }
	   else
	   {
          mClient->mCommand.mStop = false;
	   }

	   if(mClient->mCommand.mCatchup == true && mClient->mCommand.mStop == false)
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
            //float time = dist * 10.0/vel;
			float time = dist * 10.0/0.2;

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
          serverDest = serverDest * 0.2;

		  mClient->mCommand.mVelocity.x = serverDest.x;
	      mClient->mCommand.mVelocity.z = serverDest.z;

	   }

	processRotation();
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

   mShape->mOgreAnimation->updateAnimations(renderTime,mClient->mCommand.mStop);

   interpolateRotation(renderTime);
}


void ClientSidePlayer::processRotation()
{

	Ogre::Vector3 serverRotOld  = Ogre::Vector3::ZERO;
	Ogre::Vector3 serverRotNew  = Ogre::Vector3::ZERO;

	serverRotOld.x = mClient->mServerFrame.mRotOld.x;
	serverRotOld.z = mClient->mServerFrame.mRotOld.z;

	serverRotNew.x = mClient->mServerFrame.mRot.x;
	serverRotNew.z = mClient->mServerFrame.mRot.z;

	serverRotNew.normalise();
	serverRotOld.normalise();

	//calculate how far off we are from server
    Quaternion toServer = mShape->getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew,Vector3::UNIT_Y);

	// convert to degrees
	Real degreesToServer = toServer.getYaw().valueDegrees();

	// are we too far off
	if(abs(degreesToServer) > 6.0)
       mClient->mCommand.mCatchupRot = true;

	//calculate server rotation from last tick to new one
	Quaternion serverRot = mClient->mServerPlayer->mShape->getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew, Vector3::UNIT_Y);

    // convert to degrees
    serverRotSpeed = serverRot.getYaw().valueDegrees();

	// yaw server guy to new rot
	mClient->mServerPlayer->mShape->getSceneNode()->yaw(Degree(serverRotSpeed));

	//if(mClient->command.mCatchupRot == true && mClient->command.mStop == false)
	if(serverRotSpeed != 0.0 && mClient->mCommand.mCatchupRot == true)
	{
       // if server rot counter-clockwise hardcode server rot to +500
	    if(serverRotSpeed > 0.0)
          mClient->mCommand.mRotSpeed = 250.0;
	    else //clockwise - set to -500
          mClient->mCommand.mRotSpeed = -250.0;

		if(degreesToServer/serverRotSpeed > 0.0)
           mClient->mCommand.mRotSpeed = mClient->mCommand.mRotSpeed * 1.20;
		else
           mClient->mCommand.mRotSpeed = mClient->mCommand.mRotSpeed * 0.8;
	}
	else if(serverRotSpeed == 0.0 && mClient->mCommand.mCatchupRot == true)
	{
       if (degreesToServer > 0.0)
		  mClient->mCommand.mRotSpeed = 250.0;
	   else //clockwise - set to -500
          mClient->mCommand.mRotSpeed = -250.0;
	}
	else if (serverRotSpeed == 0.0)
       mClient->mCommand.mRotSpeed = 0.0;
	else
	{
       // if server rot counter-clockwise hardcode server rot to +500
	    if(serverRotSpeed > 0.0)
          mClient->mCommand.mRotSpeed = 250.0;
	   else //clockwise - set to -500
          mClient->mCommand.mRotSpeed = -250.0;
	}
}

void ClientSidePlayer::interpolateRotation(float renderTime)
{
    float rotSpeed = mClient->mCommand.mRotSpeed * renderTime;
	mShape->getSceneNode()->yaw(Degree(rotSpeed));


LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");
LogString("ghjhgjkjhkhjkhjkhjkhjkhjkhjkjhkgfhgfhfgh");




LogString("rotSpeed %f", rotSpeed);


	Ogre::Vector3 serverRotNew  = Ogre::Vector3::ZERO;

	serverRotNew.x = mClient->mServerFrame.mRot.x;
	serverRotNew.z = mClient->mServerFrame.mRot.z;

	serverRotNew.normalise();

	//calculate how far off we are from server
    Quaternion toServer = mShape->getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew,Vector3::UNIT_Y);

	// convert to degrees
	Real degreesToServer = toServer.getYaw().valueDegrees();

	// are we back in sync
	if(abs(degreesToServer) < 4.0)
       mClient->mCommand.mCatchupRot = false;

	if (serverRotSpeed == 0.0 && mClient->mCommand.mCatchupRot == false)
       mClient->mCommand.mRotSpeed = 0.0;

	LogString("degreesToServer %f", degreesToServer);


}


void ClientSidePlayer::calculateVelocity(Command* command, float frametime)
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
	   command->mVelocity.x = command->mVelocity.x/length * 0.2 * frametime * 1000;
	   command->mVelocity.z = command->mVelocity.z/length * 0.2 * frametime * 1000;
	}
}


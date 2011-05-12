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
	runSpeed  = 200.0;
	turnSpeed = 250.0;
	posInterpLimitHigh = 8.0; //how far away from server till we try to catch up
	posInterpLimitLow  = 2.0; //how close to server till we are in sync
	posInterpFactor    = 10.0;
	rotInterpLimitHigh = 6.0; //how far away from server till we try to catch up
	rotInterpLimitLow  = 4.0; //how close to server till we are in sync
	rotInterpIncrease  = 1.20; //rot factor used to catchup to server
    rotInterpDecrease  = 0.80; //rot factor used to allow server to catchup to client
}

ClientSidePlayer::~ClientSidePlayer()
{
}

void ClientSidePlayer::processTick()
{
	float deltaX = mClient->mServerFrame.mOrigin.x - mShape->getSceneNode()->getPosition().x;
    float deltaZ = mClient->mServerFrame.mOrigin.z - mShape->getSceneNode()->getPosition().z;

	//distance we are off from server
	float dist = sqrt(pow(deltaX, 2) + pow(deltaZ, 2));

	  // if distance exceeds threshold
	  if(dist > posInterpLimitHigh)
	   {
          mClient->mCommand.mCatchup = true;
	   }

	   //if we are back in sync
	   if(dist < posInterpLimitLow)
          mClient->mCommand.mCatchup = false;

	   // if server has come to a stop
	   if(mClient->mServerFrame.mVelocity.x == 0.0 && mClient->mServerFrame.mVelocity.z == 0.0)
	   {
		   mClient->mCommand.mStop = true;
	   }
	   else //server still moving
	   {
          mClient->mCommand.mStop = false;
	   }
    
	   //if server moving and client needs to catchup
	   if(mClient->mCommand.mCatchup == true && mClient->mCommand.mStop == false)
	   {
		    Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO; //vector to future server pos
		    Ogre::Vector3 myDest      = Ogre::Vector3::ZERO; //vector from clienr pos to future server pos

			serverDest.x = mClient->mServerFrame.mVelocity.x;
			serverDest.z = mClient->mServerFrame.mVelocity.z;
			serverDest.normalise();

			float multiplier = dist * posInterpFactor;
			serverDest = serverDest * multiplier;
			serverDest.x = mClient->mServerFrame.mOrigin.x + serverDest.x;
			serverDest.z = mClient->mServerFrame.mOrigin.z + serverDest.z;

			myDest.x = serverDest.x - mShape->getSceneNode()->getPosition().x;
			myDest.z = serverDest.z - mShape->getSceneNode()->getPosition().z;

            //dist from clienr pos to future server pos
			float predictDist = pow(myDest.x, 2) + pow(myDest.z, 2);
			predictDist = sqrt(predictDist);

			//server velocity
			float vel = sqrt(pow(mClient->mServerFrame.mVelocity.x, 2) + pow(mClient->mServerFrame.mVelocity.z, 2))/mClient->mCommand.mMilliseconds;
			//time needed to get to future server pos
			float time = dist * posInterpFactor/(runSpeed/1000.0);

			myDest.normalise();

			//client vel needed to get to future server pos in time
			myDest = myDest * predictDist/time;

			mClient->mCommand.mVelocity.x = myDest.x;
	        mClient->mCommand.mVelocity.z = myDest.z;
	   }
	   else //server stopped or we are in sync so just use server vel as is
	   {
          Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO;
		  Ogre::Vector3 myDest      = Ogre::Vector3::ZERO;

	      serverDest.x = mClient->mServerFrame.mVelocity.x;
		  serverDest.z = mClient->mServerFrame.mVelocity.z;
		  serverDest.normalise();
          serverDest = serverDest * runSpeed/1000.0;

		  mClient->mCommand.mVelocity.x = serverDest.x;
	      mClient->mCommand.mVelocity.z = serverDest.z;

	   }

	processRotation();
}

void ClientSidePlayer::interpolateTick(float renderTime)
{
  Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

   transVector.x = mClient->mCommand.mVelocity.x;
   transVector.z = mClient->mCommand.mVelocity.z;
   mShape->getSceneNode()->translate(transVector * renderTime * 1000, Ogre::Node::TS_WORLD);

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
	if(abs(degreesToServer) > rotInterpLimitHigh)
       mClient->mCommand.mCatchupRot = true;

	//calculate server rotation from last tick to new one
	Quaternion serverRot = mClient->mServerPlayer->mShape->getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew, Vector3::UNIT_Y);

    // convert to degrees
    serverRotSpeed = serverRot.getYaw().valueDegrees();

	// yaw server guy to new rot
	mClient->mServerPlayer->mShape->getSceneNode()->yaw(Degree(serverRotSpeed));

	//if we're rotating and need to catch up to server
	if(serverRotSpeed != 0.0 && mClient->mCommand.mCatchupRot == true)
	{
       // if server rot counter-clockwise
	    if(serverRotSpeed > 0.0)
          mClient->mCommand.mRotSpeed = turnSpeed;
	    else //clockwise
          mClient->mCommand.mRotSpeed = -turnSpeed;

		// if we are rotating counter-clockwise to catch up
		if(degreesToServer/serverRotSpeed > 0.0)
           mClient->mCommand.mRotSpeed = mClient->mCommand.mRotSpeed * rotInterpIncrease;
		// if we are rotating clockwise to catch up
		else
           mClient->mCommand.mRotSpeed = mClient->mCommand.mRotSpeed * rotInterpDecrease;
	}
	// if server is not rotating but we still need to catchup
	else if(serverRotSpeed == 0.0 && mClient->mCommand.mCatchupRot == true)
	{
	   //counter clockwise
       if (degreesToServer > 0.0)
		  mClient->mCommand.mRotSpeed = turnSpeed;
	   else //clockwise
          mClient->mCommand.mRotSpeed = -turnSpeed;
	}
	// if server is not rotating and we don't need to catch up
	else if (serverRotSpeed == 0.0)
	{
       mClient->mCommand.mRotSpeed = 0.0;
	}
	// just rotating at same speed as server
	else
	{
       // if server rot counter-clockwise
	    if(serverRotSpeed > 0.0)
          mClient->mCommand.mRotSpeed = turnSpeed;
	   else //clockwise
          mClient->mCommand.mRotSpeed = -turnSpeed;
	}
}

void ClientSidePlayer::interpolateRotation(float renderTime)
{
    float rotSpeed = mClient->mCommand.mRotSpeed * renderTime;
	mShape->getSceneNode()->yaw(Degree(rotSpeed));

	Ogre::Vector3 serverRotNew  = Ogre::Vector3::ZERO;

	serverRotNew.x = mClient->mServerFrame.mRot.x;
	serverRotNew.z = mClient->mServerFrame.mRot.z;

	serverRotNew.normalise();

	//calculate how far off we are from server
    Quaternion toServer = mShape->getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew,Vector3::UNIT_Y);

	// convert to degrees
	Real degreesToServer = toServer.getYaw().valueDegrees();

	// are we back in sync
	if(abs(degreesToServer) < rotInterpLimitLow)
       mClient->mCommand.mCatchupRot = false;

	if (serverRotSpeed == 0.0 && mClient->mCommand.mCatchupRot == false)
       mClient->mCommand.mRotSpeed = 0.0;

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


#include "moveStates.h"
#include "moveStateMachine.h"

//player,client,shape combo
#include "../clientSidePlayer.h"
#include "../../client/clientSideClient.h"
#include "../../shape/ogreShape.h"

//only using for logstring this needs to be remedied.
#include "../../tdreamsock/dreamSock.h"

Normal_Move* Normal_Move::Instance()
{
  static Normal_Move instance;
  return &instance;
}
void Normal_Move::enter(ClientSidePlayer* player)
{
	LogString("STATE: Normal_Move");
}
void Normal_Move::execute(ClientSidePlayer* player)
{
	float deltaX = player->mClient->mServerFrame.mOrigin.x - player->mShape->getSceneNode()->getPosition().x;
    float deltaZ = player->mClient->mServerFrame.mOrigin.z - player->mShape->getSceneNode()->getPosition().z;

	//distance we are off from server
	float dist = sqrt(pow(deltaX, 2) + pow(deltaZ, 2));

	// if distance exceeds threshold
	if(dist > player->posInterpLimitHigh)
	{
		player->mClient->mCommand.mCatchup = true;
	}

	//if we are back in sync
	if(dist < player->posInterpLimitLow)
	{
		player->mClient->mCommand.mCatchup = false;
	}
	   
	// if server has come to a stop
	if(player->mClient->mServerFrame.mVelocity.x == 0.0 && player->mClient->mServerFrame.mVelocity.z == 0.0)
	{
		player->mClient->mCommand.mStop = true;
	}
	else //server still moving
	{
		player->mClient->mCommand.mStop = false;
	}
    
	//if server moving and client needs to catchup change to catchup state
	if(player->mClient->mCommand.mCatchup == true && player->mClient->mCommand.mStop == false)
	{
		player->moveStateMachine->changeState(Catchup_Move::Instance());
	}
	else //server stopped or we are in sync so just use server vel as is, this is meat of normal state...
	{
		Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO;
		Ogre::Vector3 myDest      = Ogre::Vector3::ZERO;

	    serverDest.x = player->mClient->mServerFrame.mVelocity.x;
		serverDest.z = player->mClient->mServerFrame.mVelocity.z;
		serverDest.normalise();
        serverDest = serverDest * player->runSpeed/1000.0;

		player->mClient->mCommand.mVelocity.x = serverDest.x;
	    player->mClient->mCommand.mVelocity.z = serverDest.z;
	}
}
void Normal_Move::exit(ClientSidePlayer* player)
{
}

Catchup_Move* Catchup_Move::Instance()
{
  static Catchup_Move instance;
  return &instance;
}
void Catchup_Move::enter(ClientSidePlayer* player)
{
	LogString("STATE: Catchup_Move");
}
void Catchup_Move::execute(ClientSidePlayer* player)
{
	float deltaX = player->mClient->mServerFrame.mOrigin.x - player->mShape->getSceneNode()->getPosition().x;
    float deltaZ = player->mClient->mServerFrame.mOrigin.z - player->mShape->getSceneNode()->getPosition().z;

	//distance we are off from server
	float dist = sqrt(pow(deltaX, 2) + pow(deltaZ, 2));

	// if distance exceeds threshold
	if(dist > player->posInterpLimitHigh)
	{
		player->mClient->mCommand.mCatchup = true;
	}

	//if we are back in sync
	if(dist < player->posInterpLimitLow)
	{
		player->mClient->mCommand.mCatchup = false;
	}
	   
	// if server has come to a stop
	if(player->mClient->mServerFrame.mVelocity.x == 0.0 && player->mClient->mServerFrame.mVelocity.z == 0.0)
	{
		player->mClient->mCommand.mStop = true;
	}
	else //server still moving
	{
		player->mClient->mCommand.mStop = false;
	}
    
	//if server moving and client needs to catchup
	if(player->mClient->mCommand.mCatchup == true && player->mClient->mCommand.mStop == false)
	{
		Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO; //vector to future server pos
		Ogre::Vector3 myDest      = Ogre::Vector3::ZERO; //vector from clienr pos to future server pos

		serverDest.x = player->mClient->mServerFrame.mVelocity.x;
		serverDest.z = player->mClient->mServerFrame.mVelocity.z;
		serverDest.normalise();

		float multiplier = dist * player->posInterpFactor;
		serverDest = serverDest * multiplier;
		serverDest.x = player->mClient->mServerFrame.mOrigin.x + serverDest.x;
		serverDest.z = player->mClient->mServerFrame.mOrigin.z + serverDest.z;

		myDest.x = serverDest.x - player->mShape->getSceneNode()->getPosition().x;
		myDest.z = serverDest.z - player->mShape->getSceneNode()->getPosition().z;

        //dist from clienr pos to future server pos
		float predictDist = pow(myDest.x, 2) + pow(myDest.z, 2);
		predictDist = sqrt(predictDist);

		//server velocity
		float vel = sqrt(pow(player->mClient->mServerFrame.mVelocity.x, 2) + pow(player->mClient->mServerFrame.mVelocity.z, 2))/player->mClient->mCommand.mMilliseconds;
		//time needed to get to future server pos
		float time = dist * player->posInterpFactor/(player->runSpeed/1000.0);

		myDest.normalise();

		//client vel needed to get to future server pos in time
		myDest = myDest * predictDist/time;

		player->mClient->mCommand.mVelocity.x = myDest.x;
	    player->mClient->mCommand.mVelocity.z = myDest.z;
	}
	else //server stopped or we are in sync so just use server vel as is
	{
		//this is normal state...
		player->moveStateMachine->changeState(Normal_Move::Instance());
	}
}
void Catchup_Move::exit(ClientSidePlayer* player)
{
}


Stop_Move* Stop_Move::Instance()
{
  static Stop_Move instance;
  return &instance;
}
void Stop_Move::enter(ClientSidePlayer* player)
{
	LogString("STATE: Stop_Move");
}
void Stop_Move::execute(ClientSidePlayer* player)
{

	float deltaX = player->mClient->mServerFrame.mOrigin.x - player->mShape->getSceneNode()->getPosition().x;
    float deltaZ = player->mClient->mServerFrame.mOrigin.z - player->mShape->getSceneNode()->getPosition().z;

	//distance we are off from server
	float dist = sqrt(pow(deltaX, 2) + pow(deltaZ, 2));

	  // if distance exceeds threshold
	  if(dist > player->posInterpLimitHigh)
	   {
          player->mClient->mCommand.mCatchup = true;
	   }

	   //if we are back in sync
	   if(dist < player->posInterpLimitLow)
          player->mClient->mCommand.mCatchup = false;

	   // if server has come to a stop
	   if(player->mClient->mServerFrame.mVelocity.x == 0.0 && player->mClient->mServerFrame.mVelocity.z == 0.0)
	   {
		   player->mClient->mCommand.mStop = true;
	   }
	   else //server still moving
	   {
          player->mClient->mCommand.mStop = false;
	   }
    
	   //if server moving and client needs to catchup
	   if(player->mClient->mCommand.mCatchup == true && player->mClient->mCommand.mStop == false)
	   {
		    Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO; //vector to future server pos
		    Ogre::Vector3 myDest      = Ogre::Vector3::ZERO; //vector from clienr pos to future server pos

			serverDest.x = player->mClient->mServerFrame.mVelocity.x;
			serverDest.z = player->mClient->mServerFrame.mVelocity.z;
			serverDest.normalise();

			float multiplier = dist * player->posInterpFactor;
			serverDest = serverDest * multiplier;
			serverDest.x = player->mClient->mServerFrame.mOrigin.x + serverDest.x;
			serverDest.z = player->mClient->mServerFrame.mOrigin.z + serverDest.z;

			myDest.x = serverDest.x - player->mShape->getSceneNode()->getPosition().x;
			myDest.z = serverDest.z - player->mShape->getSceneNode()->getPosition().z;

            //dist from clienr pos to future server pos
			float predictDist = pow(myDest.x, 2) + pow(myDest.z, 2);
			predictDist = sqrt(predictDist);

			//server velocity
			float vel = sqrt(pow(player->mClient->mServerFrame.mVelocity.x, 2) + pow(player->mClient->mServerFrame.mVelocity.z, 2))/player->mClient->mCommand.mMilliseconds;
			//time needed to get to future server pos
			float time = dist * player->posInterpFactor/(player->runSpeed/1000.0);

			myDest.normalise();

			//client vel needed to get to future server pos in time
			myDest = myDest * predictDist/time;

			player->mClient->mCommand.mVelocity.x = myDest.x;
	        player->mClient->mCommand.mVelocity.z = myDest.z;
	   }
	   else //server stopped or we are in sync so just use server vel as is
	   {
          Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO;
		  Ogre::Vector3 myDest      = Ogre::Vector3::ZERO;

	      serverDest.x = player->mClient->mServerFrame.mVelocity.x;
		  serverDest.z = player->mClient->mServerFrame.mVelocity.z;
		  serverDest.normalise();
          serverDest = serverDest * player->runSpeed/1000.0;

		  player->mClient->mCommand.mVelocity.x = serverDest.x;
	      player->mClient->mCommand.mVelocity.z = serverDest.z;

	   }
}
void Stop_Move::exit(ClientSidePlayer* player)
{
}
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
	//LogString("STATE: Normal_Move");
}
void Normal_Move::execute(ClientSidePlayer* player)
{
	// if distance exceeds threshold
	if(player->mDeltaPosition > player->mPosInterpLimitHigh && player->mClient->mCommand.mStop == false)
	{
		player->mMoveStateMachine->changeState(Catchup_Move::Instance());
	}
    
	else //server stopped or we are in sync so just use server vel as is, this is meat of normal state...
	{
		Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO;
		Ogre::Vector3 myDest      = Ogre::Vector3::ZERO;

	    serverDest.x = player->mClient->mServerFrame.mVelocity.x;
		serverDest.z = player->mClient->mServerFrame.mVelocity.z;
		serverDest.normalise();
        serverDest = serverDest * player->mRunSpeed/1000.0;

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
	//LogString("STATE: Catchup_Move");
}
void Catchup_Move::execute(ClientSidePlayer* player)
{
	//if we are back in sync
	if(player->mDeltaPosition <= player->mPosInterpLimitHigh || player->mClient->mCommand.mStop == true)
	{
		player->mMoveStateMachine->changeState(Normal_Move::Instance());
	}

	//if server moving and client needs to catchup
	//else if(player->mClient->mCommand.mStop == false)
	//{
		Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO; //vector to future server pos
		Ogre::Vector3 myDest      = Ogre::Vector3::ZERO; //vector from clienr pos to future server pos

		serverDest.x = player->mClient->mServerFrame.mVelocity.x;
		serverDest.z = player->mClient->mServerFrame.mVelocity.z;
		serverDest.normalise();

		float multiplier = player->mDeltaPosition * player->mPosInterpFactor;
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
		float time = player->mDeltaPosition * player->mPosInterpFactor/(player->mRunSpeed/1000.0);

		myDest.normalise();

		//client vel needed to get to future server pos in time
		myDest = myDest * predictDist/time;

		player->mClient->mCommand.mVelocity.x = myDest.x;
	    player->mClient->mCommand.mVelocity.z = myDest.z;
	//}

}
void Catchup_Move::exit(ClientSidePlayer* player)
{
}


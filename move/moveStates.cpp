#include "moveStates.h"
#include "moveStateMachine.h"

#include "move.h"

//player,client,shape combo
#include "../player/clientSidePlayer.h"
#include "../client/clientSideClient.h"
#include "../shape/ogreShape.h"

//only using for logstring this needs to be remedied.
#include "../tdreamsock/dreamSock.h"

Normal_Move* Normal_Move::Instance()
{
  static Normal_Move instance;
  return &instance;
}
void Normal_Move::enter(Move* move)
{

}
void Normal_Move::execute(Move* move)
{
	// if distance exceeds threshold
	if(move->mDeltaPosition > move->mPosInterpLimitHigh && move->mPlayer->mClient->mCommand.mStop == false)
	{
		move->mMoveStateMachine->changeState(Catchup_Move::Instance());
	}
    
	else //server stopped or we are in sync so just use server vel as is, this is meat of normal state...
	{
		Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO;
		Ogre::Vector3 myDest      = Ogre::Vector3::ZERO;

	    serverDest.x = move->mPlayer->mClient->mServerFrame.mVelocity.x;
		serverDest.z = move->mPlayer->mClient->mServerFrame.mVelocity.z;
		serverDest.normalise();
        serverDest = serverDest * move->mRunSpeed/1000.0;

		move->mPlayer->mClient->mCommand.mVelocity.x = serverDest.x;
	    move->mPlayer->mClient->mCommand.mVelocity.z = serverDest.z;
	}
}
void Normal_Move::exit(Move* move)
{
}

Catchup_Move* Catchup_Move::Instance()
{
  static Catchup_Move instance;
  return &instance;
}
void Catchup_Move::enter(Move* move)
{
	//LogString("STATE: Catchup_Move");
}
void Catchup_Move::execute(Move* move)
{
	//if we are back in sync
	if(move->mDeltaPosition <= move->mPosInterpLimitHigh || move->mPlayer->mClient->mCommand.mStop == true)
	{
		move->mMoveStateMachine->changeState(Normal_Move::Instance());
	}
	else
	{

		Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO; //vector to future server pos
		Ogre::Vector3 myDest      = Ogre::Vector3::ZERO; //vector from clienr pos to future server pos

		serverDest.x = move->mPlayer->mClient->mServerFrame.mVelocity.x;
		serverDest.z = move->mPlayer->mClient->mServerFrame.mVelocity.z;
		serverDest.normalise();

		float multiplier = move->mDeltaPosition * move->mPosInterpFactor;
		serverDest = serverDest * multiplier;
		serverDest.x = move->mPlayer->mClient->mServerFrame.mOrigin.x + serverDest.x;
		serverDest.z = move->mPlayer->mClient->mServerFrame.mOrigin.z + serverDest.z;

		myDest.x = serverDest.x - move->mPlayer->mShape->getSceneNode()->getPosition().x;
		myDest.z = serverDest.z - move->mPlayer->mShape->getSceneNode()->getPosition().z;

        //dist from clienr pos to future server pos
		float predictDist = pow(myDest.x, 2) + pow(myDest.z, 2);
		predictDist = sqrt(predictDist);

		//server velocity
		float vel = sqrt(pow(move->mPlayer->mClient->mServerFrame.mVelocity.x, 2) + pow(move->mPlayer->mClient->mServerFrame.mVelocity.z, 2))/move->mPlayer->mClient->mCommand.mMilliseconds;
		//time needed to get to future server pos
		float time = move->mDeltaPosition * move->mPosInterpFactor/(move->mRunSpeed/1000.0);

		myDest.normalise();

		//client vel needed to get to future server pos in time
		myDest = myDest * predictDist/time;

		move->mPlayer->mClient->mCommand.mVelocity.x = myDest.x;
	    move->mPlayer->mClient->mCommand.mVelocity.z = myDest.z;
	}

}
void Catchup_Move::exit(Move* move)
{
}


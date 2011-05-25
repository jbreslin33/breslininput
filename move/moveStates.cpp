#include "moveStates.h"
#include "moveStateMachine.h"

#include "move.h"

//player,client,shape combo
#include "../client/client.h"
#include "../player/clientSidePlayer.h"
#include "../shape/ogreShape.h"

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
	if(move->mDeltaPosition > move->mPosInterpLimitHigh && move->mPlayer->mCommand.mStop == false)
	{
		move->mMoveStateMachine->changeState(Catchup_Move::Instance());
	}
    
	else //server stopped or we are in sync so just use server vel as is, this is meat of normal state...
	{
		Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO;
		Ogre::Vector3 myDest      = Ogre::Vector3::ZERO;

	    serverDest.x = move->mPlayer->mServerFrame.mVelocity.x;
		serverDest.z = move->mPlayer->mServerFrame.mVelocity.z;
		serverDest.y = move->mPlayer->mServerFrame.mVelocity.y;
		serverDest.normalise();

		move->mRunSpeed = 0.0;

        if(move->mPlayer->mCommand.mMilliseconds != 0)
		{
		   move->mRunSpeed = sqrt(pow(move->mPlayer->mServerFrame.mVelocity.x, 2) + 
		   pow(move->mPlayer->mServerFrame.mVelocity.z, 2) + pow(move->mPlayer->mServerFrame.mVelocity.y, 2))/move->mPlayer->mCommand.mMilliseconds;
		}

		serverDest = serverDest * move->mRunSpeed;

		move->mPlayer->mCommand.mVelocity.x = serverDest.x;
	    move->mPlayer->mCommand.mVelocity.z = serverDest.z;
		move->mPlayer->mCommand.mVelocity.y = serverDest.y;
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
	if(move->mDeltaPosition <= move->mPosInterpLimitHigh || move->mPlayer->mCommand.mStop == true)
	{
		move->mMoveStateMachine->changeState(Normal_Move::Instance());
	}
	else
	{
		Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO; //vector to future server pos
		Ogre::Vector3 myDest      = Ogre::Vector3::ZERO; //vector from clienr pos to future server pos

		serverDest.x = move->mPlayer->mServerFrame.mVelocity.x;
		serverDest.z = move->mPlayer->mServerFrame.mVelocity.z;
		serverDest.y = move->mPlayer->mServerFrame.mVelocity.y;
		serverDest.normalise();

		float multiplier = move->mDeltaPosition * move->mPosInterpFactor;
		serverDest = serverDest * multiplier;
		serverDest.x = move->mPlayer->mServerFrame.mOrigin.x + serverDest.x;
		serverDest.z = move->mPlayer->mServerFrame.mOrigin.z + serverDest.z;
		serverDest.y = move->mPlayer->mServerFrame.mOrigin.y + serverDest.y;
		//LogString("mOrigin.y %f", move->mPlayer->mClient->mServerFrame.mOrigin.y);

		myDest.x = serverDest.x - move->mPlayer->getSceneNode()->getPosition().x;
		myDest.z = serverDest.z - move->mPlayer->getSceneNode()->getPosition().z;
		myDest.y = serverDest.y - move->mPlayer->getSceneNode()->getPosition().y;

        //dist from clienr pos to future server pos
		float predictDist = pow(myDest.x, 2) + pow(myDest.z, 2) + pow(myDest.y, 2);
		predictDist = sqrt(predictDist);

		//server velocity
        move->mRunSpeed = sqrt(pow(move->mPlayer->mServerFrame.mVelocity.x, 2) + 
		   pow(move->mPlayer->mServerFrame.mVelocity.z, 2) + pow(move->mPlayer->mServerFrame.mVelocity.y, 2))/move->mPlayer->mCommand.mMilliseconds;
				
		//time needed to get to future server pos
		float time = move->mDeltaPosition * move->mPosInterpFactor/move->mRunSpeed;

		myDest.normalise();

		//client vel needed to get to future server pos in time
		myDest = myDest * predictDist/time;

		move->mPlayer->mCommand.mVelocity.x = myDest.x;
	    move->mPlayer->mCommand.mVelocity.z = myDest.z;
		move->mPlayer->mCommand.mVelocity.y = myDest.y;
	}

}
void Catchup_Move::exit(Move* move)
{
}


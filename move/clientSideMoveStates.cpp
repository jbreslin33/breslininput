#include "clientSideMoveStates.h"
#include "clientSideMoveStateMachine.h"

#include "clientSideMove.h"

Normal_Move* Normal_Move::Instance()
{
  static Normal_Move instance;
  return &instance;
}
void Normal_Move::enter(ClientSideMove* move)
{

}
void Normal_Move::execute(ClientSideMove* move)
{
	// if distance exceeds threshold
    if(move->mDeltaPosition > move->mPosInterpLimitHigh && move->mCommand.mStop == false)
    {
		move->mMoveStateMachine->changeState(Catchup_Move::Instance());
    }
    else //server stopped or we are in sync so just use server vel as is, this is meat of normal state...
    {
		Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO;
        Ogre::Vector3 myDest      = Ogre::Vector3::ZERO;

		serverDest.x = move->mServerFrame.mVelocity.x;
	    serverDest.z = move->mServerFrame.mVelocity.z;
        serverDest.y = move->mServerFrame.mVelocity.y;
        serverDest.normalise();

        move->mRunSpeed = 0.0;

        if(move->mCommand.mMilliseconds != 0)
        {
			move->mRunSpeed = sqrt(pow(move->mServerFrame.mVelocity.x, 2) + 
            pow(move->mServerFrame.mVelocity.z, 2) + pow(move->mServerFrame.mVelocity.y, 2))/move->mCommand.mMilliseconds;
        }

        serverDest = serverDest * move->mRunSpeed;

		move->mCommand.mVelocity.x = serverDest.x;
        move->mCommand.mVelocity.z = serverDest.z;
        move->mCommand.mVelocity.y = serverDest.y;
	}
}
void Normal_Move::exit(ClientSideMove* move)
{
}

Catchup_Move* Catchup_Move::Instance()
{
	static Catchup_Move instance;
	return &instance;
}
void Catchup_Move::enter(ClientSideMove* move)
{
}
void Catchup_Move::execute(ClientSideMove* move)
{
	//if we are back in sync
    if(move->mDeltaPosition <= move->mPosInterpLimitHigh || move->mCommand.mStop == true)
    {
		move->mMoveStateMachine->changeState(Normal_Move::Instance());
    }
    else
    {
		Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO; //vector to future server pos
        Ogre::Vector3 myDest      = Ogre::Vector3::ZERO; //vector from clienr pos to future server pos

        serverDest.x = move->mServerFrame.mVelocity.x;
        serverDest.z = move->mServerFrame.mVelocity.z;
        serverDest.y = move->mServerFrame.mVelocity.y;
        serverDest.normalise();

        float multiplier = move->mDeltaPosition * move->mPosInterpFactor;
        serverDest = serverDest * multiplier;
        serverDest.x = move->mServerFrame.mOrigin.x + serverDest.x;
        serverDest.z = move->mServerFrame.mOrigin.z + serverDest.z;
        serverDest.y = move->mServerFrame.mOrigin.y + serverDest.y;
                //LogString("mOrigin.y %f", move->mClient->mServerFrame.mOrigin.y);

        myDest.x = serverDest.x - move->getSceneNode()->getPosition().x;
        myDest.z = serverDest.z - move->getSceneNode()->getPosition().z;
        myDest.y = serverDest.y - move->getSceneNode()->getPosition().y;

        //dist from clienr pos to future server pos
        float predictDist = pow(myDest.x, 2) + pow(myDest.z, 2) + pow(myDest.y, 2);
        predictDist = sqrt(predictDist);

        //server velocity
        move->mRunSpeed = sqrt(pow(move->mServerFrame.mVelocity.x, 2) + 
        pow(move->mServerFrame.mVelocity.z, 2) + pow(move->mServerFrame.mVelocity.y, 2))/move->mCommand.mMilliseconds;
                                
        //time needed to get to future server pos
        float time = move->mDeltaPosition * move->mPosInterpFactor/move->mRunSpeed;

        myDest.normalise();

        //client vel needed to get to future server pos in time
        myDest = myDest * predictDist/time;

        move->mCommand.mVelocity.x = myDest.x;
        move->mCommand.mVelocity.z = myDest.z;
        move->mCommand.mVelocity.y = myDest.y;
	}
}
void Catchup_Move::exit(ClientSideMove* move)
{
}


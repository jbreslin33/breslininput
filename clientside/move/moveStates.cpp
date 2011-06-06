#include "moveStates.h"
#include "moveStateMachine.h"

#include "../../tdreamsock/dreamSockLog.h"

#include "move.h"


Global_ProcessTick_Move* Global_ProcessTick_Move::Instance()
{
  static Global_ProcessTick_Move instance;
  return &instance;
}
void Global_ProcessTick_Move::enter(Move* move)
{

}
void Global_ProcessTick_Move::execute(Move* move)
{
	move->mDeltaX = move->mServerFrame.mOrigin.x - move->getSceneNode()->getPosition().x;
    move->mDeltaZ = move->mServerFrame.mOrigin.z - move->getSceneNode()->getPosition().z;
    move->mDeltaY = move->mServerFrame.mOrigin.y - move->getSceneNode()->getPosition().y;

    //distance we are off from server
    move->mDeltaPosition = sqrt(pow(move->mDeltaX, 2) + pow(move->mDeltaZ, 2) + pow(move->mDeltaY, 2));
}
void Global_ProcessTick_Move::exit(Move* move)
{
}


Normal_ProcessTick_Move* Normal_ProcessTick_Move::Instance()
{
  static Normal_ProcessTick_Move instance;
  return &instance;
}
void Normal_ProcessTick_Move::enter(Move* move)
{

}
void Normal_ProcessTick_Move::execute(Move* move)
{
	move->mObjectTitleString.append("M:Normal ");

	// if distance exceeds threshold && server velocity is zero
	if(move->mDeltaPosition > move->mPosInterpLimitHigh && !move->mServerFrame.mVelocity.isZero())
	{
		move->mMoveProcessTickStateMachine->changeState(Catchup_ProcessTick_Move::Instance());
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
void Normal_ProcessTick_Move::exit(Move* move)
{
}

Catchup_ProcessTick_Move* Catchup_ProcessTick_Move::Instance()
{
	static Catchup_ProcessTick_Move instance;
	return &instance;
}
void Catchup_ProcessTick_Move::enter(Move* move)
{
}
void Catchup_ProcessTick_Move::execute(Move* move)
{
	move->mObjectTitleString.append("M:Catchup ");
	//if we are back in sync
    if(move->mDeltaPosition <= move->mPosInterpLimitHigh || move->mServerFrame.mVelocity.isZero())
    {
		move->mMoveProcessTickStateMachine->changeState(Normal_ProcessTick_Move::Instance());
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
void Catchup_ProcessTick_Move::exit(Move* move)
{
}


/*************************Interpolate***************/

Global_InterpolateTick_Move* Global_InterpolateTick_Move::Instance()
{
  static Global_InterpolateTick_Move instance;
  return &instance;
}
void Global_InterpolateTick_Move::enter(Move* move)
{

}
void Global_InterpolateTick_Move::execute(Move* move)
{

}
void Global_InterpolateTick_Move::exit(Move* move)
{
}


Normal_InterpolateTick_Move* Normal_InterpolateTick_Move::Instance()
{
  static Normal_InterpolateTick_Move instance;
  return &instance;
}
void Normal_InterpolateTick_Move::enter(Move* move)
{

}
void Normal_InterpolateTick_Move::execute(Move* move)
{
	Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

    transVector.x = move->mCommand.mVelocity.x;
    transVector.z = move->mCommand.mVelocity.z;
    transVector.y = move->mCommand.mVelocity.y;
        
    move->getSceneNode()->translate(transVector * move->mRenderTime * 1000, Ogre::Node::TS_WORLD);

    if(move->getSceneNode()->getPosition().y < 0.0)
	{	
		move->getSceneNode()->setPosition(move->getSceneNode()->getPosition().x, 0.0 ,
			move->getSceneNode()->getPosition().z);
	}
}
void Normal_InterpolateTick_Move::exit(Move* move)
{
}

Catchup_InterpolateTick_Move* Catchup_InterpolateTick_Move::Instance()
{
	static Catchup_InterpolateTick_Move instance;
	return &instance;
}
void Catchup_InterpolateTick_Move::enter(Move* move)
{
}
void Catchup_InterpolateTick_Move::execute(Move* move)
{

}
void Catchup_InterpolateTick_Move::exit(Move* move)
{
}


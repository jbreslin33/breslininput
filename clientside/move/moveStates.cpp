#include "moveStates.h"
#include "moveStateMachine.h"

#include "../../tdreamsock/dreamSockLog.h"

#include "move.h"

//utility
#include <math.h>



/******************** Global_ProcessTick_Move *****************/

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
	move->mObjectTitleString.append(StringConverter::toString(move->mIndex));
	move->calculateDeltaPosition();
}
void Global_ProcessTick_Move::exit(Move* move)
{
}

/******************** Normal_ProcessTick_Move *****************/

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
	//move->mObjectTitleString.append("M:Normal ");

	// if distance exceeds threshold && server velocity is zero
	if(move->mDeltaPosition > move->mPosInterpLimitHigh && !move->mServerFrame.mVelocity.isZero())
	{
		move->mMoveProcessTickStateMachine->changeState(Catchup_ProcessTick_Move::Instance());
    }
    else //server stopped or we are in sync so just use server vel as is, this is meat of normal state...
    {
		Vector3D serverDest;
       // Ogre::Vector3 myDest      = Ogre::Vector3::ZERO;

		serverDest.x = move->mServerFrame.mVelocity.x;
	    serverDest.y = move->mServerFrame.mVelocity.y;
        serverDest.z = move->mServerFrame.mVelocity.z;
        serverDest.normalise();

       // move->mRunSpeed = 0.0;

        if(move->mCommandToRunOnShape.mMilliseconds != 0)
        {
			
			move->mRunSpeed =
			sqrt(
			pow(move->mServerFrame.mVelocity.x, 2) + 
            pow(move->mServerFrame.mVelocity.y, 2) +
			pow(move->mServerFrame.mVelocity.z, 2)) /
			move->mCommandToRunOnShape.mMilliseconds;
        }

        serverDest = serverDest * move->mRunSpeed;

		move->mCommandToRunOnShape.mVelocity.x = serverDest.x;
        move->mCommandToRunOnShape.mVelocity.y = serverDest.y;
        move->mCommandToRunOnShape.mVelocity.z = serverDest.z;
	}
}
void Normal_ProcessTick_Move::exit(Move* move)
{
}

/******************** Catchup_ProcessTick_Move *****************/

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
	//move->mObjectTitleString.append("M:Catchup ");
	//if we are back in sync
    if(move->mDeltaPosition <= move->mPosInterpLimitHigh || move->mServerFrame.mVelocity.isZero())
    {
		move->mMoveProcessTickStateMachine->changeState(Normal_ProcessTick_Move::Instance());
    }
    else
    {
		Vector3D serverDest; //vector to future server pos
        Vector3D myDest; //vector from clienr pos to future server pos

        serverDest.x = move->mServerFrame.mVelocity.x;
        serverDest.y = move->mServerFrame.mVelocity.y;
        serverDest.z = move->mServerFrame.mVelocity.z;
        serverDest.normalise();

        float multiplier = move->mDeltaPosition * move->mPosInterpFactor;
        serverDest = serverDest * multiplier;
        serverDest.x = move->mServerFrame.mOrigin.x + serverDest.x;
        serverDest.y = move->mServerFrame.mOrigin.y + serverDest.y;
        serverDest.z = move->mServerFrame.mOrigin.z + serverDest.z;
                //LogString("mOrigin.y %f", move->mClient->mServerFrame.mOrigin.y);

        myDest.x = serverDest.x - move->getPosition().x;
        myDest.y = serverDest.y - move->getPosition().y;
        myDest.z = serverDest.z - move->getPosition().z;


        //dist from clienr pos to future server pos
        float predictDist = pow(myDest.x, 2) + pow(myDest.y, 2) + pow(myDest.z, 2);
        predictDist = sqrt(predictDist);

        //server velocity
		if(move->mCommandToRunOnShape.mMilliseconds != 0)
        {
           move->mRunSpeed = sqrt(pow(move->mServerFrame.mVelocity.x, 2) + 
           pow(move->mServerFrame.mVelocity.y, 2) + pow(move->mServerFrame.mVelocity.z, 2))/move->mCommandToRunOnShape.mMilliseconds;
		}

		if(move->mRunSpeed != 0.0)
		{
           //time needed to get to future server pos
           float time = move->mDeltaPosition * move->mPosInterpFactor/move->mRunSpeed;

           myDest.normalise();

           //client vel needed to get to future server pos in time
		   float distTime = predictDist/time;
           myDest = myDest * distTime;

           move->mCommandToRunOnShape.mVelocity.x = myDest.x;
           move->mCommandToRunOnShape.mVelocity.y = myDest.y;
           move->mCommandToRunOnShape.mVelocity.z = myDest.z;

		}
		else
		{
			//why would catchup ever need to set velocity to zero, wouldn't we simply leave catchup state??
           move->mCommandToRunOnShape.mVelocity.x = 0.0;
           move->mCommandToRunOnShape.mVelocity.y = 0.0;
           move->mCommandToRunOnShape.mVelocity.z = 0.0;

		}
	}
}
void Catchup_ProcessTick_Move::exit(Move* move)
{
}


/******************** Global_InterpolateTick_Move *****************/

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

/******************** Normal_InterpolateTick_Move *****************/

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
	Vector3D transVector;

    transVector.x = move->mCommandToRunOnShape.mVelocity.x;
    transVector.y = move->mCommandToRunOnShape.mVelocity.y;
    transVector.z = move->mCommandToRunOnShape.mVelocity.z;
        
    move->translate(transVector * move->mRenderTime * 1000, 1);

	//does this just prevent you from going below 0 up and down?
    if(move->getPosition().y < 0.0)
	{	
		move->setPosition(move->getPosition().x, 0.0 ,move->getPosition().z);
	}
}
void Normal_InterpolateTick_Move::exit(Move* move)
{
}

/******************** Off_InterpolateTick_Move *****************/

Off_InterpolateTick_Move* Off_InterpolateTick_Move::Instance()
{
	static Off_InterpolateTick_Move instance;
	return &instance;
}
void Off_InterpolateTick_Move::enter(Move* move)
{
}
void Off_InterpolateTick_Move::execute(Move* move)
{

}
void Off_InterpolateTick_Move::exit(Move* move)
{
}


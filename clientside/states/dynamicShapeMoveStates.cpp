#include "dynamicShapeMoveStates.h"
#include "dynamicShapeStateMachine.h"

#include "../tdreamsock/dreamSockLog.h"

#include "../shape/dynamicShape.h"

//utility
#include <math.h>



/******************** Global_ProcessTick_Move *****************/

Global_ProcessTick_Move* Global_ProcessTick_Move::Instance()
{
  static Global_ProcessTick_Move instance;
  return &instance;
}
void Global_ProcessTick_Move::enter(DynamicShape* dynamicShape)
{

}
void Global_ProcessTick_Move::execute(DynamicShape* dynamicShape)
{
	dynamicShape->appendToTitle(dynamicShape->mIndex);
	
	dynamicShape->calculateDeltaPosition();
}
void Global_ProcessTick_Move::exit(DynamicShape* dynamicShape)
{
}

/******************** Normal_ProcessTick_Move *****************/

Normal_ProcessTick_Move* Normal_ProcessTick_Move::Instance()
{
  static Normal_ProcessTick_Move instance;
  return &instance;
}
void Normal_ProcessTick_Move::enter(DynamicShape* dynamicShape)
{

}
void Normal_ProcessTick_Move::execute(DynamicShape* dynamicShape)
{
	//dynamicShape->appendToTitle("M:Normal");

	// if distance exceeds threshold && server velocity is zero
	if(dynamicShape->mDeltaPosition > dynamicShape->mPosInterpLimitHigh && !dynamicShape->mServerFrame.mVelocity.isZero())
	{
		dynamicShape->mMoveProcessTickStateMachine->changeState(Catchup_ProcessTick_Move::Instance());
    }
    else //server stopped or we are in sync so just use server vel as is, this is meat of normal state...
    {
		Vector3D serverDest;
       // Ogre::Vector3 myDest      = Ogre::Vector3::ZERO;

		serverDest.x = dynamicShape->mServerFrame.mVelocity.x;
	    serverDest.y = dynamicShape->mServerFrame.mVelocity.y;
        serverDest.z = dynamicShape->mServerFrame.mVelocity.z;
        serverDest.normalise();

       // dynamicShape->mRunSpeed = 0.0;

        if(dynamicShape->mCommandToRunOnShape.mMilliseconds != 0)
        {
			
			dynamicShape->mRunSpeed =
			sqrt(
			pow(dynamicShape->mServerFrame.mVelocity.x, 2) + 
            pow(dynamicShape->mServerFrame.mVelocity.y, 2) +
			pow(dynamicShape->mServerFrame.mVelocity.z, 2)) /
			dynamicShape->mCommandToRunOnShape.mMilliseconds;
        }

        serverDest = serverDest * dynamicShape->mRunSpeed;

		dynamicShape->mCommandToRunOnShape.mVelocity.x = serverDest.x;
        dynamicShape->mCommandToRunOnShape.mVelocity.y = serverDest.y;
        dynamicShape->mCommandToRunOnShape.mVelocity.z = serverDest.z;
	}
}
void Normal_ProcessTick_Move::exit(DynamicShape* dynamicShape)
{
}

/******************** Catchup_ProcessTick_Move *****************/

Catchup_ProcessTick_Move* Catchup_ProcessTick_Move::Instance()
{
	static Catchup_ProcessTick_Move instance;
	return &instance;
}
void Catchup_ProcessTick_Move::enter(DynamicShape* dynamicShape)
{
}
void Catchup_ProcessTick_Move::execute(DynamicShape* dynamicShape)
{
	//dynamicShape->appendToTitle("M:Catchup");

	//if we are back in sync
    if(dynamicShape->mDeltaPosition <= dynamicShape->mPosInterpLimitHigh || dynamicShape->mServerFrame.mVelocity.isZero())
    {
		dynamicShape->mMoveProcessTickStateMachine->changeState(Normal_ProcessTick_Move::Instance());
    }
    else
    {
		Vector3D serverDest; //vector to future server pos
        Vector3D myDest; //vector from clienr pos to future server pos

        serverDest.x = dynamicShape->mServerFrame.mVelocity.x;
        serverDest.y = dynamicShape->mServerFrame.mVelocity.y;
        serverDest.z = dynamicShape->mServerFrame.mVelocity.z;
        serverDest.normalise();

        float multiplier = dynamicShape->mDeltaPosition * dynamicShape->mPosInterpFactor;
        serverDest = serverDest * multiplier;
        serverDest.x = dynamicShape->mServerFrame.mOrigin.x + serverDest.x;
        serverDest.y = dynamicShape->mServerFrame.mOrigin.y + serverDest.y;
        serverDest.z = dynamicShape->mServerFrame.mOrigin.z + serverDest.z;
                //LogString("mOrigin.y %f", dynamicShape->mClient->mServerFrame.mOrigin.y);

        myDest.x = serverDest.x - dynamicShape->getPosition().x;
        myDest.y = serverDest.y - dynamicShape->getPosition().y;
        myDest.z = serverDest.z - dynamicShape->getPosition().z;


        //dist from clienr pos to future server pos
        float predictDist = pow(myDest.x, 2) + pow(myDest.y, 2) + pow(myDest.z, 2);
        predictDist = sqrt(predictDist);

        //server velocity
		if(dynamicShape->mCommandToRunOnShape.mMilliseconds != 0)
        {
           dynamicShape->mRunSpeed = sqrt(pow(dynamicShape->mServerFrame.mVelocity.x, 2) + 
           pow(dynamicShape->mServerFrame.mVelocity.y, 2) + pow(dynamicShape->mServerFrame.mVelocity.z, 2))/dynamicShape->mCommandToRunOnShape.mMilliseconds;
		}

		if(dynamicShape->mRunSpeed != 0.0)
		{
           //time needed to get to future server pos
           float time = dynamicShape->mDeltaPosition * dynamicShape->mPosInterpFactor/dynamicShape->mRunSpeed;

           myDest.normalise();

           //client vel needed to get to future server pos in time
		   float distTime = predictDist/time;
           myDest = myDest * distTime;

           dynamicShape->mCommandToRunOnShape.mVelocity.x = myDest.x;
           dynamicShape->mCommandToRunOnShape.mVelocity.y = myDest.y;
           dynamicShape->mCommandToRunOnShape.mVelocity.z = myDest.z;

		}
		else
		{
			//why would catchup ever need to set velocity to zero, wouldn't we simply leave catchup state??
           dynamicShape->mCommandToRunOnShape.mVelocity.x = 0.0;
           dynamicShape->mCommandToRunOnShape.mVelocity.y = 0.0;
           dynamicShape->mCommandToRunOnShape.mVelocity.z = 0.0;

		}
	}
}
void Catchup_ProcessTick_Move::exit(DynamicShape* dynamicShape)
{
}


/******************** Global_InterpolateTick_Move *****************/

Global_InterpolateTick_Move* Global_InterpolateTick_Move::Instance()
{
  static Global_InterpolateTick_Move instance;
  return &instance;
}
void Global_InterpolateTick_Move::enter(DynamicShape* dynamicShape)
{

}
void Global_InterpolateTick_Move::execute(DynamicShape* dynamicShape)
{

}
void Global_InterpolateTick_Move::exit(DynamicShape* dynamicShape)
{
}

/******************** Normal_InterpolateTick_Move *****************/

Normal_InterpolateTick_Move* Normal_InterpolateTick_Move::Instance()
{
  static Normal_InterpolateTick_Move instance;
  return &instance;
}
void Normal_InterpolateTick_Move::enter(DynamicShape* dynamicShape)
{

}
void Normal_InterpolateTick_Move::execute(DynamicShape* dynamicShape)
{
	Vector3D transVector;

    transVector.x = dynamicShape->mCommandToRunOnShape.mVelocity.x;
    transVector.y = dynamicShape->mCommandToRunOnShape.mVelocity.y;
    transVector.z = dynamicShape->mCommandToRunOnShape.mVelocity.z;
        
    dynamicShape->translate(transVector * dynamicShape->mRenderTime * 1000, 1);

	//does this just prevent you from going below 0 up and down?
    if(dynamicShape->getPosition().y < 0.0)
	{	
		dynamicShape->setPosition(dynamicShape->getPosition().x, 0.0 ,dynamicShape->getPosition().z);
	}
}
void Normal_InterpolateTick_Move::exit(DynamicShape* dynamicShape)
{
}

/******************** Off_InterpolateTick_Move *****************/

Off_InterpolateTick_Move* Off_InterpolateTick_Move::Instance()
{
	static Off_InterpolateTick_Move instance;
	return &instance;
}
void Off_InterpolateTick_Move::enter(DynamicShape* dynamicShape)
{
}
void Off_InterpolateTick_Move::execute(DynamicShape* dynamicShape)
{

}
void Off_InterpolateTick_Move::exit(DynamicShape* dynamicShape)
{
}


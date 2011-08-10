#include "dynamicShape.h"
#include "../tdreamsock/dreamSockLog.h"

//game
#include "../game/game.h"

#include "../states/dynamicShapeStateMachine.h"
#include "../states/dynamicShapeMoveStates.h"
#include "../states/dynamicShapeRotationStates.h"
#include "../states/dynamicShapeAnimationStates.h"

#include "../dispatch/dispatch.h"

#include "../billboard/objectTitle.h"

#include <string.h>

DynamicShape::DynamicShape(Game* game, Dispatch* dispatch)
:
	Shape         ()
{
	mGame = game;

	mPosition = new Vector3D();
	mVelocity = new Vector3D();
	mRotation = new Vector3D();

	parseDispatch(dispatch);
	initializeVariables();
	initializeCommands(mPosition,mRotation);
	createStateMachines();
}

DynamicShape::~DynamicShape()
{

}

void DynamicShape::parseDispatch(Dispatch* dispatch)
{
	dispatch->BeginReading();
	dispatch->ReadByte(); //should read -103 to add a shape..
	mLocal	=    dispatch->ReadByte();
	mIndex		=    dispatch->ReadByte();
	dispatch->ReadString(); //strcpy(name, dispatch->ReadString()); //name
	mPosition->x =   dispatch->ReadFloat();
	mPosition->y =   dispatch->ReadFloat();
	mPosition->z =   dispatch->ReadFloat();
	mVelocity->x = dispatch->ReadFloat();
	mVelocity->y = dispatch->ReadFloat();
	mVelocity->z = dispatch->ReadFloat();
	mRotation->x = dispatch->ReadFloat();
	mRotation->z = dispatch->ReadFloat();
}

void DynamicShape::initializeVariables()
{
	//dynamic
	mRenderTime = 0.0;

	//run speed
	mRunSpeed     = 0.0;
	mRunSpeedMax  = 200.0;

	mGhost = NULL;

    //thresholds
    mPosInterpLimitHigh = 8.0; //how far away from server till we try to catch up
    mPosInterpLimitLow  = 2.0; //how close to server till we are in sync
    mPosInterpFactor    = 4.0;

	//deltas
	mDeltaX        = 0.0; 
	mDeltaY		   = 0.0;
	mDeltaZ        = 0.0;
	mDeltaPosition = 0.0;

	//////rotation
    mTurnSpeed = 250.0;

    mRotInterpLimitHigh = 6.0; //how far away from server till we try to catch up
    mRotInterpLimitLow  = 4.0; //how close to server till we are in sync
    mRotInterpIncrease  = 1.20f; //rot factor used to catchup to server
    mRotInterpDecrease  = 0.80f; //rot factor used to allow server to catchup to client

	//rotation
	mServerRotOld.zero();
	mServerRotNew.zero();
	mDegreesToServer = 0.0;
}

void DynamicShape::initializeCommands(Vector3D* position, Vector3D* rotation)
{
	//let's set the serverframe...for inititialize purposeses
	mServerFrame.mOrigin.x = position->x;
	mServerFrame.mOrigin.y = position->y;
	mServerFrame.mOrigin.z = position->z;

	mServerFrame.mOriginOld.x = position->x;
	mServerFrame.mOriginOld.y = position->y;
	mServerFrame.mOriginOld.z = position->z;

	mServerFrame.mVelocity.x = position->x;
	mServerFrame.mVelocity.y = position->y;
	mServerFrame.mVelocity.z = position->z;

	mServerFrame.mRot.x = rotation->x;
	mServerFrame.mRot.y = 0;
	mServerFrame.mRot.z = rotation->z;

	mServerFrame.mMilliseconds = 0;
	mServerFrame.mMillisecondsTotal = 0;

	mServerFrame.mKey = 0;
	mServerFrame.mRotSpeed = 0;

	//let's set the serverframe...for inititialize purposeses
	mCommandToRunOnShape.mOrigin.x = position->x;
	mCommandToRunOnShape.mOrigin.y = position->y;
	mCommandToRunOnShape.mOrigin.z = position->z;

	mCommandToRunOnShape.mOriginOld.x = position->x;
	mCommandToRunOnShape.mOriginOld.y = position->y;
	mCommandToRunOnShape.mOriginOld.z = position->z;

	mCommandToRunOnShape.mVelocity.x = position->x;
	mCommandToRunOnShape.mVelocity.y = position->y;
	mCommandToRunOnShape.mVelocity.z = position->z;

	mCommandToRunOnShape.mRot.x = rotation->x;
	mCommandToRunOnShape.mRot.y = 0;
	mCommandToRunOnShape.mRot.z = rotation->z;

	mCommandToRunOnShape.mMilliseconds = 0;
	mCommandToRunOnShape.mMillisecondsTotal = 0;

	mCommandToRunOnShape.mKey = 0;
	mCommandToRunOnShape.mRotSpeed = 0;

}

void DynamicShape::createStateMachines()
{
	//move processTick states
	mMoveProcessTickStateMachine = new DynamicShapeStateMachine(this);    //setup the state machine
	mMoveProcessTickStateMachine->setCurrentState      (Normal_ProcessTick_Move::Instance());
	mMoveProcessTickStateMachine->setPreviousState     (Normal_ProcessTick_Move::Instance());
	mMoveProcessTickStateMachine->setGlobalState       (Global_ProcessTick_Move::Instance());

	//move interpolateTick states
	mMoveInterpolateTickStateMachine = new DynamicShapeStateMachine(this);    //setup the state machine
	mMoveInterpolateTickStateMachine->setCurrentState      (Normal_InterpolateTick_Move::Instance());
	mMoveInterpolateTickStateMachine->setPreviousState     (Normal_InterpolateTick_Move::Instance());
	//mMoveInterpolateTickStateMachine->setGlobalState       (Global_InterpolateTick_Move::Instance());
	mMoveInterpolateTickStateMachine->setGlobalState       (NULL);


	//process tick rotation states
	mRotationProcessTickStateMachine = new DynamicShapeStateMachine(this);    //setup the state machine
	mRotationProcessTickStateMachine->setCurrentState      (Normal_ProcessTick_Rotation::Instance());
	mRotationProcessTickStateMachine->setPreviousState     (Normal_ProcessTick_Rotation::Instance());
	mRotationProcessTickStateMachine->setGlobalState       (Global_ProcessTick_Rotation::Instance());

	//interpolate tick rotation states
	mRotationInterpolateTickStateMachine = new DynamicShapeStateMachine(this);    //setup the state machine
	mRotationInterpolateTickStateMachine->setCurrentState      (Normal_InterpolateTick_Rotation::Instance());
	mRotationInterpolateTickStateMachine->setPreviousState     (Normal_ProcessTick_Rotation::Instance());
	mRotationInterpolateTickStateMachine->setGlobalState       (Global_InterpolateTick_Rotation::Instance());

    //animation interpolateTick states
    mAnimationInterpolateTickStateMachine = new DynamicShapeStateMachine(this);    //setup the state machine
    mAnimationInterpolateTickStateMachine->setCurrentState (Idle_InterpolateTick_Animation::Instance());
    mAnimationInterpolateTickStateMachine->setPreviousState(Idle_InterpolateTick_Animation::Instance());
    mAnimationInterpolateTickStateMachine->setGlobalState  (Global_InterpolateTick_Animation::Instance());  

}

void DynamicShape::processTick()
{
	clearTitle(); //empty title string so it can be filled anew

	//update state machines...
    mMoveProcessTickStateMachine->update();
	mRotationProcessTickStateMachine->update();
		
	//run billboard here for now.
	//drawTitle();
}
void DynamicShape::interpolateTick(float renderTime)
{
	mRenderTime = renderTime;

	//update state machines...
	mMoveInterpolateTickStateMachine->update();
	mRotationInterpolateTickStateMachine->update();
	mAnimationInterpolateTickStateMachine->update();
}

float DynamicShape::getDegreesToServer()
{
    Vector3D serverRotNew;

    serverRotNew.x = mServerFrame.mRot.x;
	serverRotNew.y = 0;
    serverRotNew.z = mServerFrame.mRot.z;

    serverRotNew.normalise();

    //calculate how far off we are from server
	float degreesToServer = getDegreesToSomething(serverRotNew);

	return degreesToServer;
}

void DynamicShape::calculateServerRotationSpeed()
{
    mServerRotOld.zero();
    mServerRotNew.zero();

    mServerRotOld.x = mServerFrame.mRotOld.x;
	mServerRotOld.y = 0;
    mServerRotOld.z = mServerFrame.mRotOld.z;

    mServerRotNew.x = mServerFrame.mRot.x;
	mServerRotNew.y = 0;
    mServerRotNew.z = mServerFrame.mRot.z;

    mServerRotNew.normalise();
    mServerRotOld.normalise();

    //calculate how far off we are from server
	mDegreesToServer = getDegreesToSomething(mServerRotNew);

    //calculate server rotation from last tick to new one
	mServerRotSpeed = mGhost->getDegreesToSomething(mServerRotNew);

    if(abs(mServerRotSpeed) < 0)
    {
		mServerRotSpeed = 0.0f;
    }
}

void DynamicShape::calculateDeltaPosition()
{
	mDeltaX = mServerFrame.mOrigin.x - getPosition().x;
    mDeltaY = mServerFrame.mOrigin.y - getPosition().y;
    mDeltaZ = mServerFrame.mOrigin.z - getPosition().z;

    //distance we are off from server
    mDeltaPosition = sqrt(pow(mDeltaX, 2) + pow(mDeltaY, 2) +  pow(mDeltaZ, 2));
}


//this is all shapes coming to client game from server
//should a shape be responsible to read it's own command?????
//once we determine it's about him shouldn't we pass it off to
//shape object to handle?
void DynamicShape::readDeltaMoveCommand(Dispatch *mes)
{
	//DynamicShape* shape = NULL;

	int flags = 0;

	bool x = true;
	bool z = true;
	bool y = true;

	// Flags
	flags = mes->ReadByte();

	// Origin
	if(flags & mCommandOriginX)
	{
		mServerFrame.mOriginOld.x = mServerFrame.mOrigin.x;
		mServerFrame.mOrigin.x = mes->ReadFloat();		
	}
	else
	{
		x = false;
	}

	if(flags & mCommandOriginY)
	{
		mServerFrame.mOriginOld.y = mServerFrame.mOrigin.y;
		mServerFrame.mOrigin.y = mes->ReadFloat();
	}
	else
	{
		y = false;
	}

	if(flags & mCommandOriginZ)
	{
		mServerFrame.mOriginOld.z = mServerFrame.mOrigin.z;
		mServerFrame.mOrigin.z = mes->ReadFloat();	
	}
	else
	{
		z = false;
	}

	//set old rot
	mServerFrame.mRotOld.x = mServerFrame.mRot.x;
	mServerFrame.mRotOld.z = mServerFrame.mRot.z;

	//rotation
	if(flags & mCommandRotationX)
	{
		mServerFrame.mRot.x = mes->ReadFloat();
	}

	if(flags & mCommandRotationZ)
	{
		mServerFrame.mRot.z = mes->ReadFloat();
	}

	//milliseconds
	if (flags & mCommandMilliseconds)
	{
		mServerFrame.mMilliseconds = mes->ReadByte();
		mCommandToRunOnShape.mMilliseconds = mServerFrame.mMilliseconds;
	}

	if (mServerFrame.mMilliseconds != 0) 
	{
		if(!x && !z && !y && mServerFrame.mMilliseconds != 0)
		{
			mServerFrame.mVelocity.x = 0.0;
			mServerFrame.mVelocity.y = 0.0;
			mServerFrame.mVelocity.z = 0.0;
		}
		else
		{
			mServerFrame.mVelocity.x = mServerFrame.mOrigin.x - mServerFrame.mOriginOld.x;
			mServerFrame.mVelocity.y = mServerFrame.mOrigin.y - mServerFrame.mOriginOld.y;
			mServerFrame.mVelocity.z = mServerFrame.mOrigin.z - mServerFrame.mOriginOld.z;
		}
	}
	processTick();
	moveGhostShape();
}


void DynamicShape::moveGhostShape()
{
	Vector3D transVector;

	transVector.x = mServerFrame.mOrigin.x;
	transVector.y = 0;
	transVector.z = mServerFrame.mOrigin.z;

	if (mGhost)
	{
		mGhost->setPosition(transVector);
	}
}

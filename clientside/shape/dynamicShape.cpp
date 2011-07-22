#include "dynamicShape.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../states/dynamicShapeStateMachine.h"
#include "../states/dynamicShapeMoveStates.h"
#include "../states/dynamicShapeRotationStates.h"

#include "../../billboard/objectTitle.h"

#include <string.h>

DynamicShape::DynamicShape(Game* game,int ind, Vector3D* position, Vector3D* velocity,
						   Vector3D* rotation)
:
	Shape         (ind,position,rotation)
{

//dynamic
	mRenderTime = 0.0;

	//run speed
	mRunSpeed     = 0.0;
	mRunSpeedMax  = 200.0;

	// put character in starting spawn spot
    //mSceneNode->translate(position->x,position->z,position->y, Node::TS_LOCAL);

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
	mCommandToRunOnShape.mRot.z = rotation->z;

	mCommandToRunOnShape.mMilliseconds = 0;
	mCommandToRunOnShape.mMillisecondsTotal = 0;

	mCommandToRunOnShape.mKey = 0;
	mCommandToRunOnShape.mRotSpeed = 0;

	mGhost = NULL;

	/////move
    //thresholds

    mPosInterpLimitHigh = 8.0; //how far away from server till we try to catch up
    mPosInterpLimitLow  = 2.0; //how close to server till we are in sync
    mPosInterpFactor    = 4.0;


	//deltas
	mDeltaX        = 0.0; 
	mDeltaY		   = 0.0;
	mDeltaZ        = 0.0;
	mDeltaPosition = 0.0;

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

	//////rotation
    //vars
    mTurnSpeed = 250.0;

    mRotInterpLimitHigh = 6.0; //how far away from server till we try to catch up
    mRotInterpLimitLow  = 4.0; //how close to server till we are in sync
    mRotInterpIncrease  = 1.20f; //rot factor used to catchup to server
    mRotInterpDecrease  = 0.80f; //rot factor used to allow server to catchup to client

	//rotation
	mServerRotOld.zero();
	mServerRotNew.zero();
	mDegreesToServer = 0.0;

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
}

DynamicShape::~DynamicShape()
{

}

void DynamicShape::processTick()
{

	//mObjectTitleString.clear(); //empty title string so it can be filled anew

	//call parent processTicks since you overrode them
    mMoveProcessTickStateMachine->update();
	mRotationProcessTickStateMachine->update();
	//OgreAnimation::processTick();
	
	//run billboard here for now.
	//mObjectTitle->setTitle(mObjectTitleString); //fill title with mObjectTitleString which we have appended from all states we want info on.
	//mObjectTitle->update(); //draw title for shape

}
void DynamicShape::interpolateTick(float renderTime)
{
	mRenderTime = renderTime;

	//call parent interpolateTicks since you overrode them
	mMoveInterpolateTickStateMachine->update();
	mRotationInterpolateTickStateMachine->update();
	//OgreAnimation::interpolateTick();
}

float DynamicShape::getDegreesToServer()
{
    Vector3D serverRotNew;

    serverRotNew.x = mServerFrame.mRot.x;
    serverRotNew.z = mServerFrame.mRot.z;

    serverRotNew.normalise();

    //calculate how far off we are from server
   // Quaternion toServer = getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew,Vector3::UNIT_Y);


    // convert to degrees
    //Real degreesToServer = toServer.getYaw().valueDegrees();
	float degreesToServer = getDegreesToSomething(serverRotNew);
	return degreesToServer;
}

void DynamicShape::calculateServerRotationSpeed()
{
    mServerRotOld.zero();
    mServerRotNew.zero();

    mServerRotOld.x = mServerFrame.mRotOld.x;
    mServerRotOld.z = mServerFrame.mRotOld.z;

    mServerRotNew.x = mServerFrame.mRot.x;
    mServerRotNew.z = mServerFrame.mRot.z;

    mServerRotNew.normalise();
    mServerRotOld.normalise();

    //calculate how far off we are from server
    //Quaternion toServer = getSceneNode()->getOrientation().zAxis().getRotationTo(mServerRotNew,Vector3::UNIT_Y);

    // convert to degrees
    //mDegreesToServer = toServer.getYaw().valueDegrees();

	mDegreesToServer = getDegreesToSomething(mServerRotNew);

    //calculate server rotation from last tick to new one
	
    //Quaternion serverRot = mGhost->getSceneNode()->getOrientation().zAxis().getRotationTo(mServerRotNew, Vector3::UNIT_Y);

    // convert to degrees
    //mServerRotSpeed = serverRot.getYaw().valueDegrees();
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
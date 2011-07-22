#include "rotation.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../clientside/shape/dynamicShape.h"

#include <string>

//rotation states
#include "rotationStateMachine.h"
#include "rotationStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16

Rotation::Rotation()
:
	Shape()
{
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
	mRotationProcessTickStateMachine = new RotationStateMachine(this);    //setup the state machine
	mRotationProcessTickStateMachine->setCurrentState      (Normal_ProcessTick_Rotation::Instance());
	mRotationProcessTickStateMachine->setPreviousState     (Normal_ProcessTick_Rotation::Instance());
	mRotationProcessTickStateMachine->setGlobalState       (Global_ProcessTick_Rotation::Instance());

	//interpolate tick rotation states
	mRotationInterpolateTickStateMachine = new RotationStateMachine(this);    //setup the state machine
	mRotationInterpolateTickStateMachine->setCurrentState      (Normal_InterpolateTick_Rotation::Instance());
	mRotationInterpolateTickStateMachine->setPreviousState     (Normal_ProcessTick_Rotation::Instance());
	mRotationInterpolateTickStateMachine->setGlobalState       (Global_InterpolateTick_Rotation::Instance());
}

Rotation::~Rotation()
{
	delete mRotationProcessTickStateMachine;
	delete mRotationInterpolateTickStateMachine;
}

void Rotation::processTick()
{
    mRotationProcessTickStateMachine->update();
}

void Rotation::interpolateTick()
{
	mRotationInterpolateTickStateMachine->update();	
}

float Rotation::getDegreesToServer()
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

void Rotation::calculateServerRotationSpeed()
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
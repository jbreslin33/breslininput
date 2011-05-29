#include "clientSideRotation.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../shape/clientSideShape.h"

#include <string>

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//rotation states
#include "rotationStateMachine.h"
#include "rotationStates.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16

ClientSideRotation::ClientSideRotation(std::string name, Vector3D* position,
									   Ogre::SceneManager* mSceneMgr, std::string mesh)
 : OgreShape(name, position,mSceneMgr,mesh), Rotation()
{
	//mShape = player;

	mServerShape = NULL;

	mTurnSpeed = 250.0;

	mRotInterpLimitHigh = 6.0; //how far away from server till we try to catch up
	mRotInterpLimitLow  = 4.0; //how close to server till we are in sync
	mRotInterpIncrease  = 1.20f; //rot factor used to catchup to server
    mRotInterpDecrease  = 0.80f; //rot factor used to allow server to catchup to client

	//rotation
	mServerRotOld  = Ogre::Vector3::ZERO;
	mServerRotNew  = Ogre::Vector3::ZERO;
	mDegreesToServer = 0.0;

	//rotation states
	mRotationStateMachine = new RotationStateMachine(this);    //setup the state machine
	mRotationStateMachine->setCurrentState      (Normal_Rotation::Instance());
	mRotationStateMachine->setPreviousState     (Normal_Rotation::Instance());
	mRotationStateMachine->setGlobalState       (NULL);
	//mClientSideRotationStateMachine->changeState        (Normal_ClientSideRotation::Instance());

}

ClientSideRotation::~ClientSideRotation()
{
}

void ClientSideRotation::processTick()
{
	
        mServerRotOld  = Ogre::Vector3::ZERO;
    mServerRotNew  = Ogre::Vector3::ZERO;

    mServerRotOld.x = mServerFrame.mRotOld.x;
    mServerRotOld.z = mServerFrame.mRotOld.z;

    mServerRotNew.x = mServerFrame.mRot.x;
    mServerRotNew.z = mServerFrame.mRot.z;

    mServerRotNew.normalise();
    mServerRotOld.normalise();

    //calculate how far off we are from server
    Quaternion toServer = getSceneNode()->getOrientation().zAxis().getRotationTo(mServerRotNew,Vector3::UNIT_Y);

    // convert to degrees
    mDegreesToServer = toServer.getYaw().valueDegrees();

    //calculate server rotation from last tick to new one
    Quaternion serverRot = mServerShape->getSceneNode()->getOrientation().zAxis().getRotationTo(mServerRotNew, Vector3::UNIT_Y);

    // convert to degrees
    mServerRotSpeed = serverRot.getYaw().valueDegrees();

    if(abs(mServerRotSpeed) < 0.01)
        {
                mServerRotSpeed = 0.0;
        }
    
        //LogString("mServerRotSpeed %f", mServerRotSpeed);

    // yaw server guy to new rot
    mServerShape->getSceneNode()->yaw(Degree(mServerRotSpeed));

        mRotationStateMachine->update();
		
}

void ClientSideRotation::interpolateTick(float renderTime)
{
	
    float rotSpeed = mCommand.mRotSpeed * renderTime;
        getSceneNode()->yaw(Degree(rotSpeed));

        Ogre::Vector3 serverRotNew  = Ogre::Vector3::ZERO;

        serverRotNew.x = mServerFrame.mRot.x;
        serverRotNew.z = mServerFrame.mRot.z;

        serverRotNew.normalise();

        //calculate how far off we are from server
    Quaternion toServer = getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew,Vector3::UNIT_Y);

        // convert to degrees
        Real mDegreesToServer = toServer.getYaw().valueDegrees();

        // are we back in sync
        if(abs(mDegreesToServer) < mRotInterpLimitLow)
        {
       mCommand.mCatchupRot = false;
        }

        if (mServerRotSpeed == 0.0 && mCommand.mCatchupRot == false)
        {
       mCommand.mRotSpeed = 0.0;
        }
		
}


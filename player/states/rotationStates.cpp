#include "rotationStates.h"
#include "rotationStateMachine.h"

//player,client,shape combo
#include "../clientSidePlayer.h"
#include "../../client/clientSideClient.h"
#include "../../shape/ogreShape.h"

//only using for logstring this needs to be remedied.
#include "../../tdreamsock/dreamSock.h"

Normal_Rotation* Normal_Rotation::Instance()
{
  static Normal_Rotation instance;
  return &instance;
}
void Normal_Rotation::enter(ClientSidePlayer* player)
{
	LogString("STATE: Normal_Rotation");
}
void Normal_Rotation::execute(ClientSidePlayer* player)
{

	Ogre::Vector3 serverRotOld  = Ogre::Vector3::ZERO;
	Ogre::Vector3 serverRotNew  = Ogre::Vector3::ZERO;

	serverRotOld.x = player->mClient->mServerFrame.mRotOld.x;
	serverRotOld.z = player->mClient->mServerFrame.mRotOld.z;

	serverRotNew.x = player->mClient->mServerFrame.mRot.x;
	serverRotNew.z = player->mClient->mServerFrame.mRot.z;

	serverRotNew.normalise();
	serverRotOld.normalise();

	//calculate how far off we are from server
    Quaternion toServer = player->mShape->getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew,Vector3::UNIT_Y);

	// convert to degrees
	Real degreesToServer = toServer.getYaw().valueDegrees();

	// are we too far off
	if(abs(degreesToServer) > player->mRotInterpLimitHigh)
       player->mClient->mCommand.mCatchupRot = true;

	//calculate server rotation from last tick to new one
	Quaternion serverRot = player->mClient->mServerPlayer->mShape->getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew, Vector3::UNIT_Y);

    // convert to degrees
    player->mServerRotSpeed = serverRot.getYaw().valueDegrees();

	if(abs(player->mServerRotSpeed) < 0.01)
	{
		player->mServerRotSpeed = 0.0;
	}

	// yaw server guy to new rot
	player->mClient->mServerPlayer->mShape->getSceneNode()->yaw(Degree(player->mServerRotSpeed));

	//if we're rotating and need to catch up to server
	if(player->mServerRotSpeed != 0.0 && player->mClient->mCommand.mCatchupRot == true)
	{
       // if server rot counter-clockwise
	    if(player->mServerRotSpeed > 0.0)
          player->mClient->mCommand.mRotSpeed = player->mTurnSpeed;
	    else //clockwise
          player->mClient->mCommand.mRotSpeed = -player->mTurnSpeed;

		// if we are rotating counter-clockwise to catch up
		if(degreesToServer/player->mServerRotSpeed > 0.0)
           player->mClient->mCommand.mRotSpeed = player->mClient->mCommand.mRotSpeed * player->mRotInterpIncrease;
		// if we are rotating clockwise to catch up
		else
           player->mClient->mCommand.mRotSpeed = player->mClient->mCommand.mRotSpeed * player->mRotInterpDecrease;
	}
	// if server is not rotating but we still need to catchup
	else if(player->mServerRotSpeed == 0.0 && player->mClient->mCommand.mCatchupRot == true)
	{
	   //counter clockwise
       if (degreesToServer > 0.0)
		  player->mClient->mCommand.mRotSpeed = player->mTurnSpeed;
	   else //clockwise
          player->mClient->mCommand.mRotSpeed = -player->mTurnSpeed;
	}
	// if server is not rotating and we don't need to catch up
	else if (player->mServerRotSpeed == 0.0)
	{
       player->mClient->mCommand.mRotSpeed = 0.0;
	}
	// just rotating at same speed as server
	else
	{
       // if server rot counter-clockwise
	    if(player->mServerRotSpeed > 0.0)
          player->mClient->mCommand.mRotSpeed = player->mTurnSpeed;
	   else //clockwise
          player->mClient->mCommand.mRotSpeed = -player->mTurnSpeed;
	}
}
void Normal_Rotation::exit(ClientSidePlayer* player)
{
}

Catchup_Rotation* Catchup_Rotation::Instance()
{
  static Catchup_Rotation instance;
  return &instance;
}
void Catchup_Rotation::enter(ClientSidePlayer* player)
{
	LogString("STATE: Catchup_Rotation");
}
void Catchup_Rotation::execute(ClientSidePlayer* player)
{
}
void Catchup_Rotation::exit(ClientSidePlayer* player)
{
}


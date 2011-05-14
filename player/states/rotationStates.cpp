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
	ClientSideClient* mClient = player->mClient;

       Ogre::Vector3 serverRotOld  = Ogre::Vector3::ZERO;
        Ogre::Vector3 serverRotNew  = Ogre::Vector3::ZERO;

        serverRotOld.x = mClient->mServerFrame.mRotOld.x;
        serverRotOld.z = mClient->mServerFrame.mRotOld.z;

        serverRotNew.x = mClient->mServerFrame.mRot.x;
        serverRotNew.z = mClient->mServerFrame.mRot.z;

        serverRotNew.normalise();
        serverRotOld.normalise();

        //calculate how far off we are from server
    Quaternion toServer = player->mShape->getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew,Vector3::UNIT_Y);

        // convert to degrees
        Real mDegreesToServer = toServer.getYaw().valueDegrees();

        // are we too far off
        if(abs(mDegreesToServer) > 6.0)
       mClient->mCommand.mCatchupRot = true;

        //calculate server rotation from last tick to new one
        Quaternion serverRot = mClient->mServerPlayer->mShape->getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew, Vector3::UNIT_Y);

    // convert to degrees
    player->mServerRotSpeed = serverRot.getYaw().valueDegrees();

        if(abs(player->mServerRotSpeed) < 0.01)
                player->mServerRotSpeed = 0.0;

        //LogString("player->mServerRotSpeed %f", player->mServerRotSpeed);

        // yaw server guy to new rot
        mClient->mServerPlayer->mShape->getSceneNode()->yaw(Degree(player->mServerRotSpeed));





        //if(mClient->command.mCatchupRot == true && mClient->command.mStop == false)
        if(player->mServerRotSpeed != 0.0 && mClient->mCommand.mCatchupRot == true)
        {
			LogString("catchup1");
       // if server rot counter-clockwise hardcode server rot to +500
            if(player->mServerRotSpeed > 0.0)
          mClient->mCommand.mRotSpeed = 250.0;
            else //clockwise - set to -500
          mClient->mCommand.mRotSpeed = -250.0;

                if(mDegreesToServer/player->mServerRotSpeed > 0.0)
           mClient->mCommand.mRotSpeed = mClient->mCommand.mRotSpeed * 1.20;
                else
           mClient->mCommand.mRotSpeed = mClient->mCommand.mRotSpeed * 0.8;
        }
        else if(player->mServerRotSpeed == 0.0 && mClient->mCommand.mCatchupRot == true)
        {
			LogString("catchup2");
       if (mDegreesToServer > 0.0)
                  mClient->mCommand.mRotSpeed = 250.0;
           else //clockwise - set to -500
          mClient->mCommand.mRotSpeed = -250.0;
        }
        else if (player->mServerRotSpeed == 0.0)
       mClient->mCommand.mRotSpeed = 0.0;
        else
        {
       // if server rot counter-clockwise hardcode server rot to +500
            if(player->mServerRotSpeed > 0.0)
          mClient->mCommand.mRotSpeed = 250.0;
           else //clockwise - set to -500
          mClient->mCommand.mRotSpeed = -250.0;
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
	ClientSideClient* mClient = player->mClient;

        // are we too far off
        if(abs(player->mDegreesToServer) > 6.0)
       mClient->mCommand.mCatchupRot = true;

        //if(mClient->command.mCatchupRot == true && mClient->command.mStop == false)
        if(player->mServerRotSpeed != 0.0 && mClient->mCommand.mCatchupRot == true)
        {
			LogString("catchup1");
       // if server rot counter-clockwise hardcode server rot to +500
            if(player->mServerRotSpeed > 0.0)
          mClient->mCommand.mRotSpeed = 250.0;
            else //clockwise - set to -500
          mClient->mCommand.mRotSpeed = -250.0;

                if(player->mDegreesToServer/player->mServerRotSpeed > 0.0)
           mClient->mCommand.mRotSpeed = mClient->mCommand.mRotSpeed * 1.20;
                else
           mClient->mCommand.mRotSpeed = mClient->mCommand.mRotSpeed * 0.8;
        }
        else if(player->mServerRotSpeed == 0.0 && mClient->mCommand.mCatchupRot == true)
        {
			LogString("catchup2");
       if (player->mDegreesToServer > 0.0)
                  mClient->mCommand.mRotSpeed = 250.0;
           else //clockwise - set to -500
          mClient->mCommand.mRotSpeed = -250.0;
        }
        else if (player->mServerRotSpeed == 0.0)
       mClient->mCommand.mRotSpeed = 0.0;
        else
        {
       // if server rot counter-clockwise hardcode server rot to +500
            if(player->mServerRotSpeed > 0.0)
          mClient->mCommand.mRotSpeed = 250.0;
           else //clockwise - set to -500
          mClient->mCommand.mRotSpeed = -250.0;
        }

}
void Catchup_Rotation::exit(ClientSidePlayer* player)
{
}


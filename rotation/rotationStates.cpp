#include "rotationStates.h"
#include "rotationStateMachine.h"

#include "rotation.h"

//player,client,shape combo
#include "../player/clientSidePlayer.h"
#include "../shape/ogreShape.h"

Normal_Rotation* Normal_Rotation::Instance()
{
  static Normal_Rotation instance;
  return &instance;
}
void Normal_Rotation::enter(Rotation* rotation)
{
        //LogString("STATE: Normal_Rotation");
}
void Normal_Rotation::execute(Rotation* rotation)
{
        // are we too far off
    if(abs(rotation->mDegreesToServer) > rotation->mRotInterpLimitHigh)
        {
                rotation->mPlayer->mCommand.mCatchupRot = true;
                rotation->mRotationStateMachine->changeState(Catchup_Rotation::Instance());
        }
        else
        {
                if (rotation->mServerRotSpeed == 0.0)
                {
                        rotation->mPlayer->mCommand.mRotSpeed = 0.0;
                }
                else
                {
                        // if server rot counter-clockwise hardcode server rot to +mTurnSpeed
                        if(rotation->mServerRotSpeed > 0.0)
                        {
                                rotation->mPlayer->mCommand.mRotSpeed = rotation->mTurnSpeed;
                        }
                        else //clockwise - set to -mTurnSpeed
                        {
                                rotation->mPlayer->mCommand.mRotSpeed = -rotation->mTurnSpeed;
                        }
                }
        }
}
void Normal_Rotation::exit(Rotation* rotation)
{
}

Catchup_Rotation* Catchup_Rotation::Instance()
{
  static Catchup_Rotation instance;
  return &instance;
}
void Catchup_Rotation::enter(Rotation* rotation)
{
        //LogString("STATE: Catchup_Rotation");
}
void Catchup_Rotation::execute(Rotation* rotation)
{
    // are we back on track
    if(abs(rotation->mDegreesToServer) < rotation->mRotInterpLimitLow)
        {
                rotation->mPlayer->mCommand.mCatchupRot = false;
                rotation->mRotationStateMachine->changeState(Normal_Rotation::Instance());
        }
        else
        {

                //if(mClient->command.mCatchupRot == true && mClient->command.mStop == false)
                if(rotation->mServerRotSpeed != 0.0)
                {
                        // if server rot counter-clockwise hardcode server rot to +mTurnSpeed
                        if(rotation->mServerRotSpeed > 0.0)
                        {
                                rotation->mPlayer->mCommand.mRotSpeed = rotation->mTurnSpeed;
                        }
                        else //clockwise - set to -mTurnSpeed
                        {
                                rotation->mPlayer->mCommand.mRotSpeed = -rotation->mTurnSpeed;
                        }

                        if(rotation->mDegreesToServer/rotation->mServerRotSpeed > 0.0)
                        {
                                rotation->mPlayer->mCommand.mRotSpeed = rotation->mPlayer->mCommand.mRotSpeed * rotation->mRotInterpIncrease;
                        }
                        else
                        {
                                rotation->mPlayer->mCommand.mRotSpeed = rotation->mPlayer->mCommand.mRotSpeed * rotation->mRotInterpDecrease;
                        }
                }
                else if(rotation->mServerRotSpeed == 0.0)
                {
                         if (rotation->mDegreesToServer > 0.0)
                        {
                                rotation->mPlayer->mCommand.mRotSpeed = rotation->mTurnSpeed;
                        }
                        else //clockwise - set to -mTurnSpeed
                        {
                                rotation->mPlayer->mCommand.mRotSpeed = -rotation->mTurnSpeed;
                        }
                }
        }
}
void Catchup_Rotation::exit(Rotation* rotation)
{
}


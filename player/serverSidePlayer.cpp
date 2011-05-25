#include "serverSidePlayer.h"
#include "../tdreamsock/dreamSockLog.h"
#include "../client/client.h"

#include "../shape/ogreShape.h"

#include "../math/vector3D.h"
#include <math.h>

#define MAX_RUN_SPEED 500           // character running speed in units per second
#define TURN_SPEED 250.0f      // character turning in degrees per second

#define KEY_UP                                  1
#define KEY_DOWN                                2
#define KEY_LEFT                                4
#define KEY_RIGHT                               8
#define KEY_SPACE                           16

ServerSidePlayer::ServerSidePlayer(std::string name, Client* client, OgreShape* shape) : Player(name)
{
	mClient = client;
    mShape  = shape;

    mKeyDirection = Vector3::ZERO;
	mGoalDirection = Vector3::ZERO;
        mJumping  = false;
        mRunSpeed = 0.0;
        mRunAccel    = 5.0;
        mRunDecel    = 5.0;
        mJumpAccel   = 300.0;
        mGravity     = 900.0;
}

ServerSidePlayer::~ServerSidePlayer()
{
}

void ServerSidePlayer::processTick()
{
        float clientFrametime;

        clientFrametime = mCommand.mMilliseconds / 1000.0f;;

        //calculateVelocity(&mClient->mCommand, clientFrametime);

        setKeyDirection();

        mGoalDirection = Vector3::ZERO;   // we will calculate this
        Real yawAtSpeed;

        if (mKeyDirection.isZeroLength())
        {
                if(mRunSpeed > 0.0)
                   mRunSpeed -= mRunDecel;
                else
           mRunSpeed = 0.0;
        }
        else
        {
                mGoalDirection += mKeyDirection.z * Vector3::UNIT_Z;
                mGoalDirection += mKeyDirection.x * Vector3::UNIT_X;


                mGoalDirection.y = 0;
                mGoalDirection.normalise();

                Quaternion toGoal = mShape->mSceneNode->getOrientation().zAxis().getRotationTo(mGoalDirection,Vector3::UNIT_Y);

                // calculate how much the character has to turn to face goal direction
                Real yawToGoal = toGoal.getYaw().valueDegrees();

                // this is how much the character CAN turn this frame
                if(yawToGoal == 0.0)
                        yawAtSpeed = 0.0;
                else
                   yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * clientFrametime * TURN_SPEED;
        
                // turn as much as we can, but not more than we need to
                if (yawToGoal < 0) 
                        yawToGoal = std::min<Real>(0, std::max<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, yawAtSpeed, 0);
                else if (yawToGoal > 0)
                        yawToGoal = std::max<Real>(0, std::min<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, 0, yawAtSpeed);
                        
                mShape->mSceneNode->yaw(Degree(yawToGoal));

                //LogString("x %f", mShape->mSceneNode->getOrientation().zAxis().x);
                //LogString("y %f", mShape->mSceneNode->getOrientation().zAxis().y);
                //LogString("z %f", mShape->mSceneNode->getOrientation().zAxis().z);
                
                if(mRunSpeed < MAX_RUN_SPEED)
                        mRunSpeed += mRunAccel;

        }

        // move in current body direction (not the goal direction)
                mShape->mSceneNode->translate(0, 0, clientFrametime * mRunSpeed,
                        Node::TS_LOCAL);

                if(mJumping)
                {
                        LogString("mJumping");
                   mShape->mSceneNode->translate(0, clientFrametime * mVerticalVelocity, 0, Node::TS_LOCAL);
                   mVerticalVelocity -= mGravity * clientFrametime;

                   if(mShape->mSceneNode->getPosition().y < 0.0)
                   {
                           LogString("if");
              mShape->mSceneNode->setPosition(mShape->mSceneNode->getPosition().x, 0.0, mShape->mSceneNode->getPosition().z);
              mVerticalVelocity = 0.0;
                          mJumping = false;
                   }
                }

        //if(mClient->mCommand.mVelocity.x != 0.0 || mClient->mCommand.mVelocity.z != 0.0)
        //{
           mCommand.mVelocity.x = mShape->mSceneNode->getPosition().x - mCommand.mOrigin.x;
           mCommand.mVelocity.z = mShape->mSceneNode->getPosition().z - mCommand.mOrigin.z;
           mCommand.mVelocity.y = mShape->mSceneNode->getPosition().y - mCommand.mOrigin.y;
        //}

        mCommand.mOrigin.x = mShape->mSceneNode->getPosition().x;
        mCommand.mOrigin.z = mShape->mSceneNode->getPosition().z;
        mCommand.mOrigin.y = mShape->mSceneNode->getPosition().y;

        mCommand.mRot.x = mShape->mSceneNode->getOrientation().zAxis().x;
        mCommand.mRot.z = mShape->mSceneNode->getOrientation().zAxis().z;

        int f = mClient->GetIncomingSequence() & (COMMAND_HISTORY_SIZE-1);
        mProcessedFrame = f;
}
void ServerSidePlayer::calculateVelocity(Command* command, float frametime)
{

        command->mVelocity.x = 0.0f;
        command->mVelocity.z = 0.0f;

        if(command->mKey & KEY_UP)
        {
                command->mVelocity.z += -1;
        }

        if(command->mKey & KEY_DOWN)
        {
        command->mVelocity.z += 1;
        }

        if(command->mKey & KEY_LEFT)
        {
                command->mVelocity.x += -1;
        }

        if(command->mKey & KEY_RIGHT)
        {
                command->mVelocity.x += 1;
        }

        float length = sqrt(pow(command->mVelocity.x, 2) + pow(command->mVelocity.z, 2));
        if(length != 0.0)
        {
           command->mVelocity.x = command->mVelocity.x/length * MAX_RUN_SPEED * frametime;
           command->mVelocity.z = command->mVelocity.z/length * MAX_RUN_SPEED * frametime;
        }
}


void ServerSidePlayer::setKeyDirection()
{
        if(mShape) 
        {
                mKeyDirection.x = 0;
                mKeyDirection.z = 0;
                mKeyDirection.y = 0;
                
                // keep track of the player's intended direction
                if(mCommand.mKey & KEY_UP) 
                        mKeyDirection.z += -1;

                if(mCommand.mKey & KEY_LEFT) 
                        mKeyDirection.x += -1;
                
                if(mCommand.mKey & KEY_DOWN) 
           mKeyDirection.z += 1;

                if(mCommand.mKey & KEY_RIGHT) 
                        mKeyDirection.x += 1;

                if(mCommand.mKey & KEY_SPACE) 
                {
                        if(!mJumping)
                                startJump();
                }

        }
}

void ServerSidePlayer::startJump()
{
        //LogString("startJump");
   mJumping = true;
   mVerticalVelocity = mJumpAccel;
   
}

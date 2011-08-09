#include "shape.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../../serverside/client/client.h"

#include <string>

Shape::Shape(Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::Root* root,unsigned int index)
:
	Rotation(),
	Move    (),
	Jump    (),
	AI      (),
	OgreShape		  (position,velocity,rotation,root,index)
{
	//client if this shape has associated with it
	mClient = NULL;
}

Shape::~Shape()
{
}

void Shape::processTick()
{
	setKeyDirection();

	//give ai a chance to jump in if this shape has not client
	if (mClient == NULL)
	{
		AI::processTick();
	}

	//don't do any rotation if there is no mKey set
	//should we though? we do it for movement below
	//does rotation not get to do the same?
	//is this why the rotation gets off???
	//UPDATE--I now check for mKey inside Rotation State machine
    
	Rotation::processTick();

	//even though there is no mKey set we still need to move as
	//brian has put in a deceleration factor so let's let it run...
	Move::processTick();

	//jump is a good work in progress but it's now it's own thing
	Jump::processTick();



	//set all vars to be sent off to clients playing on internets
	//none of this actually moves anything on server it is what is
	//going to be sent to clients so it's not in move or rotation.
	//though it could be i guess, velocity and position could go in move
	//and mRot could go in rotation.
    mCommand.mVelocity.x = mSceneNode->getPosition().x - mCommand.mOrigin.x;
    mCommand.mVelocity.y = mSceneNode->getPosition().y - mCommand.mOrigin.y;
    mCommand.mVelocity.z = mSceneNode->getPosition().z - mCommand.mOrigin.z;

	mCommand.mOriginOld.x = mCommand.mOrigin.x;
    mCommand.mOriginOld.z = mCommand.mOrigin.z;
    mCommand.mOriginOld.y = mCommand.mOrigin.y;

    mCommand.mOrigin.x = mSceneNode->getPosition().x;
    mCommand.mOrigin.y = mSceneNode->getPosition().y;
    mCommand.mOrigin.z = mSceneNode->getPosition().z;


    mCommand.mRot.x = mSceneNode->getOrientation().zAxis().x;
    mCommand.mRot.z = mSceneNode->getOrientation().zAxis().z;

}

void Shape::setKeyDirection()
{
	mKeyDirection.x = 0;
    mKeyDirection.y = 0;
    mKeyDirection.z = 0;

                
    // keep track of the player's intended direction
    if(mCommand.mKey & KEY_UP) 
	{
		mKeyDirection.z += -1;
	}
    if(mCommand.mKey & KEY_LEFT) 
	{
		mKeyDirection.x += -1;
	}
                
    if(mCommand.mKey & KEY_DOWN) 
	{
        mKeyDirection.z += 1;
	}
    if(mCommand.mKey & KEY_RIGHT) 
	{
		mKeyDirection.x += 1;
	}
}
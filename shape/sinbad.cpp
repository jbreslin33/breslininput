#include "sinbad.h"
#include "../tdreamsock/dreamSock.h"

#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
Sinbad::Sinbad(ClientSideClient* client, Ogre::SceneManager* sceneMgr, std::string shapeName,int x, int y, int z,std::string meshName) : ClientSideShape(client, sceneMgr,shapeName,x,y,z,meshName)
{
	setupModel();
	setupAnimations();
}

Sinbad::~Sinbad()
{
}

void Sinbad::setupModel()
{
	Shape::setupModel();
	mKeyDirection = Vector3::ZERO;
}

void Sinbad::setupAnimations()
{
		
	// this is very important due to the nature of the exported animations
	mSceneManager->getEntity(this->mShapeName)->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
	String animNames[] =
	{"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
	"SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

	// populate our animation list
	for (int i = 0; i < NUM_ANIMS; i++)
	{
		mAnims[i] = mSceneManager->getEntity(this->mShapeName)->getAnimationState(animNames[i]);
		mAnims[i]->setLoop(true);
		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}

	// start off in the idle state (top and bottom together)
	setBaseAnimation(ANIM_IDLE_BASE,false);
	setTopAnimation(ANIM_IDLE_TOP,false);

	// relax the hands since we're not holding anything
	mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
}

void Sinbad::addTime(Real deltaTime)
{
	updateBody(deltaTime);
	updateAnimations(deltaTime);
}

void Sinbad::updateBody(Real deltaTime)
{
	getSceneNode()->translate(mKeyDirection * deltaTime * RUN_SPEED, Ogre::Node::TS_WORLD);
}

void Sinbad::updateAnimations(Real deltaTime)
{
	Real baseAnimSpeed = 1;
	Real topAnimSpeed = 1;

	mTimer += deltaTime;

	if (mClient->command.mStop == false)
	{
		if (mBaseAnimID == ANIM_IDLE_BASE)
	    {
		   // start running if not already moving and the player wants to move
		   setBaseAnimation(ANIM_RUN_BASE, true);
		   if (mTopAnimID == ANIM_IDLE_TOP) 
		   {
				setTopAnimation(ANIM_RUN_TOP, true);
		   } 
		}
	}
	else
	{
	    if (mBaseAnimID == ANIM_RUN_BASE)
	    {
		   // stop running if already moving and the player doesn't want to move
		   setBaseAnimation(ANIM_IDLE_BASE, true);
		   if (mTopAnimID == ANIM_RUN_TOP) 
		   {
				  setTopAnimation(ANIM_IDLE_TOP, true);
		   } 
		}
	}
	
	// increment the current base and top animation times
	if (mBaseAnimID != ANIM_NONE)
	{
		mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
	}
	if (mTopAnimID != ANIM_NONE)
	{
		mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);
	}

	// apply smooth transitioning between our animations
	fadeAnimations(deltaTime);
}

void Sinbad::fadeAnimations(Real deltaTime)
{
	for (int i = 0; i < NUM_ANIMS; i++)
	{
		if (mFadingIn[i])
		{
			// slowly fade this animation in until it has full weight
			Real newWeight = mAnims[i]->getWeight() + deltaTime * ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight >= 1)
			{
				mFadingIn[i] = false;
			}
		}
		else if (mFadingOut[i])
		{
			// slowly fade this animation out until it has no weight, and then disable it
			Real newWeight = mAnims[i]->getWeight() - deltaTime * ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight <= 0)
			{
				mAnims[i]->setEnabled(false);
				mFadingOut[i] = false;
			}
		}
	}
}


void Sinbad::setBaseAnimation(AnimID id, bool reset)
{
	if (mBaseAnimID >= 0 && mBaseAnimID < NUM_ANIMS)
	{
		// if we have an old animation, fade it out
		mFadingIn[mBaseAnimID] = false;
		mFadingOut[mBaseAnimID] = true;
	}

	mBaseAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset)
		{
			mAnims[id]->setTimePosition(0);
		}
	}
}

void Sinbad::setTopAnimation(AnimID id, bool reset)
{
	if (mTopAnimID >= 0 && mTopAnimID < NUM_ANIMS)
	{
		// if we have an old animation, fade it out
		mFadingIn[mTopAnimID] = false;
		mFadingOut[mTopAnimID] = true;
	}

	mTopAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset)
		{
			mAnims[id]->setTimePosition(0);
		}
	}
}

void Sinbad::ProcessRotation()
{

	Ogre::Vector3 serverRotOld  = Ogre::Vector3::ZERO;
	Ogre::Vector3 serverRotNew  = Ogre::Vector3::ZERO;

	serverRotOld.x = mClient->serverFrame.mVelocityOld.x;
	serverRotOld.z = mClient->serverFrame.mVelocityOld.z;

	serverRotNew.x = mClient->serverFrame.mVelocity.x;
	serverRotNew.z = mClient->serverFrame.mVelocity.z;

	//calculate how far off we are from server
    Quaternion toServer = mClient->mPlayer->getSceneNode()->getOrientation().zAxis().getRotationTo(serverRotNew,Vector3::UNIT_Y);

	// convert to degrees
	Real degreesToServer = toServer.getYaw().valueDegrees();

	// are we too far off
	if(abs(degreesToServer) > 10.0)
       mClient->command.mCatchupRot = true;

    // are we back in sync
	if(abs(degreesToServer) < 5.0)
       mClient->command.mCatchupRot = false;

	//calculate server rotation from last tick to new one
    Quaternion serverRot = serverRotOld.getRotationTo(serverRotNew, Vector3::UNIT_Y);

    // convert to degrees
    Real serverRotSpeed = serverRot.getYaw().valueDegrees();

	// yaw server guy to new rot
	mClient->mServerPlayer->getSceneNode()->yaw(Degree(serverRotSpeed));

	// if server rot counter-clockwise hardcode server rot to +10 degrees
	if(serverRotSpeed > 0.0)
       serverRotSpeed = 10.0;
	else //clockwise - set to -10 degrees
       serverRotSpeed = -10.0;
}

void Sinbad::ProcessTick()
{
	float deltaX = mClient->serverFrame.mOrigin.x - mClient->mPlayer->getSceneNode()->getPosition().x;
    float deltaZ = mClient->serverFrame.mOrigin.z - mClient->mPlayer->getSceneNode()->getPosition().z;

	float dist = sqrt(pow(deltaX, 2) + pow(deltaZ, 2));

	  if(dist > 8.0)
	   {
          mClient->command.mCatchup = true; 
	   }

	   if(dist < 2.0)
          mClient->command.mCatchup = false;

	   if(mClient->serverFrame.mVelocity.x == 0.0 && mClient->serverFrame.mVelocity.z == 0.0)
	   {
		   mClient->command.mStop = true;
	   }
	   else
	   {
          mClient->command.mStop = false;
	   }
	     
	   if(mClient->command.mCatchup == true && mClient->command.mStop == false)
	   {
		    Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO;
		    Ogre::Vector3 myDest      = Ogre::Vector3::ZERO;
		   
			serverDest.x = mClient->serverFrame.mVelocity.x;
			serverDest.z = mClient->serverFrame.mVelocity.z;
			serverDest.normalise();

			float multiplier = dist * 10.0;
			serverDest = serverDest * multiplier;
			serverDest.x = mClient->serverFrame.mOrigin.x + serverDest.x;
			serverDest.z = mClient->serverFrame.mOrigin.z + serverDest.z;

			myDest.x = serverDest.x - mClient->mPlayer->getSceneNode()->getPosition().x;
			myDest.z = serverDest.z - mClient->mPlayer->getSceneNode()->getPosition().z;


			float predictDist = pow(myDest.x, 2) + pow(myDest.z, 2);
			predictDist = sqrt(predictDist);

			myDest.normalise();

			float vel = sqrt(pow(mClient->serverFrame.mVelocity.x, 2) + pow(mClient->serverFrame.mVelocity.z, 2))/mClient->command.mMilliseconds;
            float time = dist * 10.0/vel;

			myDest = myDest * predictDist/time;

			mClient->command.mVelocity.x = myDest.x;
	        mClient->command.mVelocity.z = myDest.z;
	   }
	   else
	   {
          Ogre::Vector3 serverDest  = Ogre::Vector3::ZERO;
		  Ogre::Vector3 myDest      = Ogre::Vector3::ZERO;
		   
	      serverDest.x = mClient->serverFrame.mVelocity.x;
		  serverDest.z = mClient->serverFrame.mVelocity.z;
		  serverDest.normalise();
          serverDest = serverDest * 0.1;

		  mClient->command.mVelocity.x = serverDest.x;
	      mClient->command.mVelocity.z = serverDest.z;

	   }

	ProcessRotation();
}

void Sinbad::interpolateTick(float renderTime)
{
   Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

   if(mClient->command.mCatchup == true && mClient->command.mStop == false)
   {
	   transVector.x = mClient->command.mVelocity.x;
	   transVector.z = mClient->command.mVelocity.z;
	   mClient->mPlayer->getSceneNode()->translate(transVector * renderTime * 1000, Ogre::Node::TS_WORLD);
   }
   else
   {
	  transVector.x = mClient->command.mVelocity.x;
	  transVector.z = mClient->command.mVelocity.z;
	  mClient->mPlayer->getSceneNode()->translate(transVector * renderTime * 1000, Ogre::Node::TS_WORLD);

   }

   transVector.normalise();

   Ogre::Quaternion toGoal = mClient->mPlayer->getSceneNode()->getOrientation().zAxis().getRotationTo(transVector);
   Ogre::Real yawToGoal = toGoal.getYaw().valueDegrees();
   mClient->mPlayer->getSceneNode()->yaw(Ogre::Degree(yawToGoal));

   mClient->mPlayer->updateAnimations(renderTime);
}

void Sinbad::CalculateVelocity(ClientSideCommand *command, float frametime)
{
	float multiplier = 100.0f;

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
	   command->mVelocity.x = command->mVelocity.x/length * 0.1 * frametime * 1000;
	   command->mVelocity.z = command->mVelocity.z/length * 0.1 * frametime * 1000;
	}
 }

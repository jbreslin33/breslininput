#include "sinbad.h"
#include "../tdreamsock/dreamSock.h"

#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
Sinbad::Sinbad(ClientSideClient* client, Ogre::SceneManager* sceneMgr, std::string shapeName,int x, int y, int z,std::string meshName) : ClientSideShape(sceneMgr,shapeName,x,y,z,meshName)
{
	mClient = client;
	setupAnimations();
}

Sinbad::~Sinbad()
{
}

void Sinbad::setupModel()
{
	ClientSideShape::setupModel();

	// create swords and attach to sheath
	mSword1 = mSceneManager->createEntity("SinbadSword1", "Sword.mesh");
	mSword2 = mSceneManager->createEntity("SinbadSword2", "Sword.mesh");
	mEntity->attachObjectToBone("Sheath.L", mSword1);
	mEntity->attachObjectToBone("Sheath.R", mSword2);

	// create a couple of ribbon trails for the swords, just for fun
	NameValuePairList params;
	params["numberOfChains"] = "2";
	params["maxElements"] = "80";
	mSwordTrail = (RibbonTrail*)mSceneManager->createMovableObject("RibbonTrail", &params);
	mSwordTrail->setMaterialName("Examples/LightRibbonTrail");
	mSwordTrail->setTrailLength(20);
	mSwordTrail->setVisible(false);
	mSceneManager->getRootSceneNode()->attachObject(mSwordTrail);

	for (int i = 0; i < 2; i++)
	{
		mSwordTrail->setInitialColour(i, 1, 0.8, 0);
		mSwordTrail->setColourChange(i, 0.75, 1.25, 1.25, 1.25);
		mSwordTrail->setWidthChange(i, 1);
		mSwordTrail->setInitialWidth(i, 0.5);
	}

	mVerticalVelocity = 0;
	mKeyDirection = Vector3::ZERO;
}

void Sinbad::setupAnimations()
	{
		// this is very important due to the nature of the exported animations
		mEntity->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
		String animNames[] =
		{"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
		"SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

		// populate our animation list
		for (int i = 0; i < NUM_ANIMS; i++)
		{
			mAnims[i] = mEntity->getAnimationState(animNames[i]);
			mAnims[i]->setLoop(true);
			mFadingIn[i] = false;
			mFadingOut[i] = false;
		}

		// start off in the idle state (top and bottom together)
		setBaseAnimation(ANIM_IDLE_BASE,false);
		setTopAnimation(ANIM_IDLE_TOP,false);

		// relax the hands since we're not holding anything
		mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);

		mSwordsDrawn = false;
	}

void Sinbad::addTime(Real deltaTime)
{
	updateBody(deltaTime);
	updateAnimations(deltaTime);
	//updateCamera(deltaTime);
}

void Sinbad::injectKeyDown(const OIS::KeyEvent& evt)
	{

		if (evt.key == OIS::KC_Q && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
		{
			// take swords out (or put them back, since it's the same animation but reversed)
			setTopAnimation(ANIM_DRAW_SWORDS, true);
			mTimer = 0;
		}
		else if (evt.key == OIS::KC_E && !mSwordsDrawn)
		{
			if (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)
			{
				// start dancing
				setBaseAnimation(ANIM_DANCE, true);
				setTopAnimation(ANIM_NONE,false);
				// disable hand animation because the dance controls hands
				mAnims[ANIM_HANDS_RELAXED]->setEnabled(false);
			}
			else if (mBaseAnimID == ANIM_DANCE)
			{
				// stop dancing
				setBaseAnimation(ANIM_IDLE_BASE,false);
				setTopAnimation(ANIM_IDLE_TOP,false);
				// re-enable hand animation
				mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
			}
		}

		// keep track of the player's intended direction
		else if (evt.key == OIS::KC_I) mKeyDirection.z = +1;
		else if (evt.key == OIS::KC_J) mKeyDirection.x = +1;
		else if (evt.key == OIS::KC_K) mKeyDirection.z = -1;
		else if (evt.key == OIS::KC_L) mKeyDirection.x = -1;

		else if (evt.key == OIS::KC_SPACE && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
		{
			// jump if on ground
			setBaseAnimation(ANIM_JUMP_START, true);
			setTopAnimation(ANIM_NONE,false);
			mTimer = 0;
		}

		if (!mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_IDLE_BASE)
		{
			// start running if not already moving and the player wants to move
			setBaseAnimation(ANIM_RUN_BASE, true);
			if (mTopAnimID == ANIM_IDLE_TOP) setTopAnimation(ANIM_RUN_TOP, true);
		}
		
	}

void Sinbad::injectKeyUp(const OIS::KeyEvent& evt)
	{
		
		// keep track of the player's intended direction
		if (evt.key == OIS::KC_I && mKeyDirection.z == 1) mKeyDirection.z = 0;
		else if (evt.key == OIS::KC_J && mKeyDirection.x == 1) mKeyDirection.x = 0;
		else if (evt.key == OIS::KC_K && mKeyDirection.z == -1) mKeyDirection.z = 0;
		else if (evt.key == OIS::KC_L && mKeyDirection.x == -1) mKeyDirection.x = 0;

		if (mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_RUN_BASE)
		{
			// stop running if already moving and the player doesn't want to move
			setBaseAnimation(ANIM_IDLE_BASE,false);
			if (mTopAnimID == ANIM_RUN_TOP,false) setTopAnimation(ANIM_IDLE_TOP,false);
		}
		
	}

void Sinbad::updateBody(Real deltaTime)
	{

		getSceneNode()->translate(mKeyDirection * deltaTime * RUN_SPEED, Ogre::Node::TS_WORLD);

/*
		mGoalDirection = Vector3::ZERO;   // we will calculate this

		if (mKeyDirection != Vector3::ZERO && mBaseAnimID != ANIM_DANCE)
		{
			// calculate actually goal direction in world based on player's key directions
			mGoalDirection += mKeyDirection.z * mCameraNode->getOrientation().zAxis();
			mGoalDirection += mKeyDirection.x * mCameraNode->getOrientation().xAxis();
			mGoalDirection.y = 0;
			mGoalDirection.normalise();

			Quaternion toGoal = mBodyNode->getOrientation().zAxis().getRotationTo(mGoalDirection);

			// calculate how much the character has to turn to face goal direction
			Real yawToGoal = toGoal.getYaw().valueDegrees();
			// this is how much the character CAN turn this frame
			Real yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * deltaTime * TURN_SPEED;
			// reduce "turnability" if we're in midair
			if (mBaseAnimID == ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

			// turn as much as we can, but not more than we need to
			if (yawToGoal < 0) yawToGoal = std::min<Real>(0, std::max<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, yawAtSpeed, 0);
			else if (yawToGoal > 0) yawToGoal = std::max<Real>(0, std::min<Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Real>(yawToGoal, 0, yawAtSpeed);
			
			mBodyNode->yaw(Degree(yawToGoal));

			// move in current body direction (not the goal direction)
			mBodyNode->translate(0, 0, deltaTime * RUN_SPEED * mAnims[mBaseAnimID]->getWeight(),
				Node::TS_LOCAL);
		}

		if (mBaseAnimID == ANIM_JUMP_LOOP)
		{
			// if we're jumping, add a vertical offset too, and apply gravity
			mBodyNode->translate(0, mVerticalVelocity * deltaTime, 0, Node::TS_LOCAL);
			mVerticalVelocity -= GRAVITY * deltaTime;
			
			Vector3 pos = mBodyNode->getPosition();
			if (pos.y <= CHAR_HEIGHT)
			{
				// if we've hit the ground, change to landing state
				pos.y = CHAR_HEIGHT;
				mBodyNode->setPosition(pos);
				setBaseAnimation(ANIM_JUMP_END, true);
				mTimer = 0;
			}
		}
*/
	}

void Sinbad::updateAnimations(Real deltaTime)
	{
		Real baseAnimSpeed = 1;
		Real topAnimSpeed = 1;

		mTimer += deltaTime;

		if (mTopAnimID == ANIM_DRAW_SWORDS)
		{
			// flip the draw swords animation if we need to put it back
			topAnimSpeed = mSwordsDrawn ? -1 : 1;

			// half-way through the animation is when the hand grasps the handles...
			if (mTimer >= mAnims[mTopAnimID]->getLength() / 2 &&
				mTimer - deltaTime < mAnims[mTopAnimID]->getLength() / 2)
			{
				// so transfer the swords from the sheaths to the hands
				mEntity->detachAllObjectsFromBone();
				mEntity->attachObjectToBone(mSwordsDrawn ? "Sheath.L" : "Handle.L", mSword1);
				mEntity->attachObjectToBone(mSwordsDrawn ? "Sheath.R" : "Handle.R", mSword2);
				// change the hand state to grab or let go
				mAnims[ANIM_HANDS_CLOSED]->setEnabled(!mSwordsDrawn);
				mAnims[ANIM_HANDS_RELAXED]->setEnabled(mSwordsDrawn);

				// toggle sword trails
				if (mSwordsDrawn)
				{
					mSwordTrail->setVisible(false);
					mSwordTrail->removeNode(mSword1->getParentNode());
					mSwordTrail->removeNode(mSword2->getParentNode());
				}
				else
				{
					mSwordTrail->setVisible(true);
					mSwordTrail->addNode(mSword1->getParentNode());
					mSwordTrail->addNode(mSword2->getParentNode());
				}
			}

			if (mTimer >= mAnims[mTopAnimID]->getLength())
			{
				// animation is finished, so return to what we were doing before
				if (mBaseAnimID == ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP,false);
				else
				{
					setTopAnimation(ANIM_RUN_TOP,false);
					mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
				}
				mSwordsDrawn = !mSwordsDrawn;
			}
		}
		else if (mTopAnimID == ANIM_SLICE_VERTICAL || mTopAnimID == ANIM_SLICE_HORIZONTAL)
		{
			if (mTimer >= mAnims[mTopAnimID]->getLength())
			{
				// animation is finished, so return to what we were doing before
				if (mBaseAnimID == ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP,false);
				else
				{
					setTopAnimation(ANIM_RUN_TOP,false);
					mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
				}
			}

			// don't sway hips from side to side when slicing. that's just embarrasing.
			if (mBaseAnimID == ANIM_IDLE_BASE) baseAnimSpeed = 0;
		}
		else if (mBaseAnimID == ANIM_JUMP_START)
		{
			if (mTimer >= mAnims[mBaseAnimID]->getLength())
			{
				// takeoff animation finished, so time to leave the ground!
				setBaseAnimation(ANIM_JUMP_LOOP, true);
				// apply a jump acceleration to the character
				mVerticalVelocity = JUMP_ACCEL;
			}
		}
		else if (mBaseAnimID == ANIM_JUMP_END)
		{
			if (mTimer >= mAnims[mBaseAnimID]->getLength())
			{
				// safely landed, so go back to running or idling
				if (mKeyDirection == Vector3::ZERO)
				{
					setBaseAnimation(ANIM_IDLE_BASE,false);
					setTopAnimation(ANIM_IDLE_TOP,false);
				}
				else
				{
					setBaseAnimation(ANIM_RUN_BASE, true);
					setTopAnimation(ANIM_RUN_TOP, true);
				}
			}
		}

		if (mClient->command.mStop == false)
		{
			if (mBaseAnimID == ANIM_IDLE_BASE)
		    {
			   // start running if not already moving and the player wants to move
			   setBaseAnimation(ANIM_RUN_BASE, true);
			   if (mTopAnimID == ANIM_IDLE_TOP) 
				  setTopAnimation(ANIM_RUN_TOP, true);
		    }
		}
		else
		{
		    if (mBaseAnimID == ANIM_RUN_BASE)
		    {
			   // stop running if already moving and the player doesn't want to move
			   setBaseAnimation(ANIM_IDLE_BASE, true);
			   if (mTopAnimID == ANIM_RUN_TOP) 
				  setTopAnimation(ANIM_IDLE_TOP, true);
		    }

		}
	
		// increment the current base and top animation times
		if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
		if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

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
				if (newWeight >= 1) mFadingIn[i] = false;
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
			if (reset) mAnims[id]->setTimePosition(0);
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
			if (reset) mAnims[id]->setTimePosition(0);
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
	if(abs(degreesToServer) > 6.0)
       mClient->command.mCatchupRot = true;

    // are we back in sync
	if(abs(degreesToServer) < 2.0)
       mClient->command.mCatchupRot = false;

	//calculate server rotation from last tick to new one
    Quaternion serverRot = serverRotOld.getRotationTo(serverRotNew, Vector3::UNIT_Y);

    // convert to degrees
    Real serverRotSpeed = serverRot.getYaw().valueDegrees();

	//LogString("serverRotSpeed %f", serverRotSpeed);

	// yaw server guy to new rot
	mClient->mServerPlayer->getSceneNode()->yaw(Degree(serverRotSpeed));


	//if(mClient->command.mCatchupRot == true && mClient->command.mStop == false)
	if(serverRotSpeed != 0.0 && mClient->command.mCatchupRot == true)
	{
       // if server rot counter-clockwise hardcode server rot to +2 degrees
	    if(serverRotSpeed > 0.0)
          serverRotSpeed = 500.0;
	   else //clockwise - set to -10 degrees
          serverRotSpeed = -500.0;
	}
	else if (serverRotSpeed == 0.0)
       serverRotSpeed = 0.0;
	else
	{
       // if server rot counter-clockwise hardcode server rot to +2 degrees
	    if(serverRotSpeed > 0.0)
          serverRotSpeed = 500.0;
	   else //clockwise - set to -10 degrees
          serverRotSpeed = -500.0;
	}

	mClient->command.mRotSpeed = serverRotSpeed;
	
	//Real rotSpeed;
	
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

void Sinbad::interpolateRotation(float renderTime)
{
    float rotSpeed = mClient->command.mRotSpeed * renderTime;
	mClient->mPlayer->getSceneNode()->yaw(Degree(rotSpeed));
}

void Sinbad::interpolateTick(float renderTime)
{
   Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

   if(mClient->command.mCatchup == true && mClient->command.mStop == false)
   {
	   transVector.x = mClient->command.mVelocity.x;
	   transVector.z = mClient->command.mVelocity.z;
	   mClient->mPlayer->getSceneNode()->translate(transVector * renderTime * 1000, Ogre::Node::TS_WORLD);
	   //LogString("catchup");
	   //LogString("catchup mVelocity.x %f", mClient->command.mVelocity.x);
	   //LogString("catchup mVelocity.z %f", mClient->command.mVelocity.z);
   }
   else
   {
	  //CalculateVelocity(&mClient->command, renderTime);
	  transVector.x = mClient->command.mVelocity.x;
	  transVector.z = mClient->command.mVelocity.z;
	  //mClient->mPlayer->getSceneNode()->translate(transVector, Ogre::Node::TS_WORLD);
	  mClient->mPlayer->getSceneNode()->translate(transVector * renderTime * 1000, Ogre::Node::TS_WORLD);
      //LogString("normalllllllllllllllllllllllllllll");
	  //LogString("normal mVelocity.x %f", mClient->command.mVelocity.x);
	  //LogString("normal mVelocity.z %f", mClient->command.mVelocity.z);
   }

/*
   transVector.normalise();

   Ogre::Quaternion toGoal = mClient->mPlayer->getSceneNode()->getOrientation().zAxis().getRotationTo(transVector);
   Ogre::Real yawToGoal = toGoal.getYaw().valueDegrees();
   mClient->mPlayer->getSceneNode()->yaw(Ogre::Degree(yawToGoal));
*/

   mClient->mPlayer->updateAnimations(renderTime);

   interpolateRotation(renderTime);
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

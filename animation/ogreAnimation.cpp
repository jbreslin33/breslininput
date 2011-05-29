#include "ogreAnimation.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../math/vector3D.h"



OgreAnimation::OgreAnimation(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr,
								   std::string mesh) : OgreShape(name,position,mSceneMgr,mesh)
{
	mEntity = NULL;
	mStop = true;
	//setupAnimations();
}

OgreAnimation::~OgreAnimation()
{
}



void OgreAnimation::setupAnimations(Entity* entity)
	{
		mEntity = entity;
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

	}

void OgreAnimation::addTime(Real deltaTime)
{

}

void OgreAnimation::updateAnimations(Real Yvelocity, Real deltaTime, bool stop, float animSpeed)
{
	mStop = stop;

	Real baseAnimSpeed = animSpeed;
	Real topAnimSpeed = animSpeed;

	mTimer += deltaTime;

	if (mStop == false)
	{
		if(Yvelocity != 0.0 && mBaseAnimID != ANIM_JUMP_LOOP)
		{
			setBaseAnimation(ANIM_JUMP_LOOP, true);
			setTopAnimation(ANIM_NONE, true);
		}
		else if (mBaseAnimID == ANIM_IDLE_BASE || mBaseAnimID == ANIM_JUMP_LOOP)
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

void OgreAnimation::fadeAnimations(Real deltaTime)
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

void OgreAnimation::setBaseAnimation(AnimID id, bool reset)
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

void OgreAnimation::setTopAnimation(AnimID id, bool reset)
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


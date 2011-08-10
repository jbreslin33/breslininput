#include "ogreDynamicShape.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../client/client.h"

//animation states for enter method
#include "../states/dynamicShapeAnimationStates.h"

#include "../game/game.h"

#include "../billboard/objectTitle.h"

#include <string.h>

	OgreDynamicShape::OgreDynamicShape(Game* game, Dispatch* dispatch, bool isGhost)
:
	DynamicShape         (game,dispatch)
{
	//we use this to name shape. as ogre is picky about same names. it also serves as a counter of sorts.

	mIsGhost = isGhost;

	if (mIsGhost)
	{
		mIndex = mIndex * -1;
	}	

	createShape();
	setupAnimations();
	setupTitle();

	//call create ghost here..
	if (!mIsGhost) 
	{
		//create a ghost for this shape
		mGhost = new OgreDynamicShape(mGame,dispatch,true);
		mGhost->setVisible(false);

		//put shape and ghost in game vectors so they can be looped and game now knows of them.
		mGame->mShapeVector.push_back(this);
		mGame->mShapeGhostVector.push_back(mGhost);	
	}
}

OgreDynamicShape::~OgreDynamicShape()
{
	delete mObjectTitle;
	//delete mEntity;
	delete mSceneNode;
}

void OgreDynamicShape::createShape()
{
	/*********  create shape ***************/
	//mMeshName     = mesh;
	mMeshName = "sinbad.mesh";
	mName         = StringConverter::toString(mIndex);
    mSceneNode    = mGame->getSceneManager()->getRootSceneNode()->createChildSceneNode();

	//set Starting position of sceneNode, we will attach our mesh to this. this is all that's needed for static shapes. actually we need to add
	//rotation for them
	mSceneNode->setPosition(mPosition->x,mPosition->y,mPosition->z);	
	
	//create mesh
	mEntity = mGame->getSceneManager()->createEntity(mName, mMeshName);

	//attache mesh to scenenode, henceforward we will use mSceneNode to control shape.
    mSceneNode->attachObject(mEntity);

	//for scale, won't be needed in future hopefully...
	Vector3D v;
	v.x = 30;
	v.y = 30;
	v.z = 30;
	scale(v);
}

void OgreDynamicShape::setupAnimations()
{
	/*********  setup animations ***************/
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

void OgreDynamicShape::setupTitle()
{
	/*********  setup title/billboard ***************/
	const Ogre::String& titlename = "tn" + StringConverter::toString(mIndex);
	const Ogre::String& title = "ti" + StringConverter::toString(mIndex);
	const Ogre::String& fontName = "SdkTrays/Caption";
	const Ogre::ColourValue& color = Ogre::ColourValue::White;
	mObjectTitle = new ObjectTitle
	(titlename, mEntity, mGame->getSceneManager()->getCamera("PlayerCam"), title,
    fontName, color);
}

void OgreDynamicShape::scale(Vector3D scaleVector)
{
	getSceneNode()->scale(scaleVector.x, scaleVector.y, scaleVector.z);
}

void OgreDynamicShape::checkExtents(Vector3D min)
{

	Ogre::Vector3 max;
	max = Ogre::Vector3::UNIT_SCALE;

	assert( (min.x <= max.x && min.y <= max.y && min.z <= max.z) &&
                "you have a problem with a vector maybe dividing by zero or a garbage value!" );

			//mExtent = EXTENT_FINITE;
			//mMinimum = min;
			//mMaximum = max;
}

void OgreDynamicShape::yaw(float amountToYaw, bool convertToDegree)
{
	if (convertToDegree)
	{
		getSceneNode()->yaw(Degree(amountToYaw));
	}
}
float OgreDynamicShape::getDegreesToSomething(Vector3D vectorOfSomething)
{
    //calculate how far off we are from server
    Quaternion toSomething = getSceneNode()->getOrientation().zAxis().getRotationTo(converToVector3(vectorOfSomething),Vector3::UNIT_Y);

    // convert to degrees
    Real degreesToServer = toSomething.getYaw().valueDegrees();
	return degreesToServer;
}

//1 world, 2 local
void OgreDynamicShape::translate(Vector3D translateVector, int perspective)
{
	if (perspective == 1)
	{
		getSceneNode()->translate(converToVector3(translateVector), Ogre::Node::TS_WORLD);
	}
	if (perspective == 2)
	{
		getSceneNode()->translate(converToVector3(translateVector), Ogre::Node::TS_LOCAL);
	}
}

void OgreDynamicShape::setPosition(Vector3D position)
{
	getSceneNode()->setPosition(converToVector3(position));
}

void OgreDynamicShape::setPosition(float x, float y, float z)
{
	getSceneNode()->setPosition(x,y,z);
}

Vector3D OgreDynamicShape::getPosition()
{
	Vector3D position;
	position.x = getSceneNode()->getPosition().x;
	position.y = getSceneNode()->getPosition().y;
	position.z = getSceneNode()->getPosition().z;
	return position;
}

Ogre::Vector3 OgreDynamicShape::converToVector3(Vector3D vector3d)
{
	Ogre::Vector3 vec3;
	vec3.x = vector3d.x;
	vec3.y = vector3d.y;
	vec3.z = vector3d.z;
	return vec3;
}

void OgreDynamicShape::setVisible(bool visible)
{
	getSceneNode()->setVisible(visible);
}

//animation
void OgreDynamicShape::runAnimations()
{
	mAnims[mBaseAnimID]->addTime(mRenderTime * mRunSpeed * 1000/mRunSpeedMax);
	mAnims[mTopAnimID]->addTime(mRenderTime * mRunSpeed * 1000/mRunSpeedMax);
	fadeAnimations(mRenderTime);

	fadeAnimations(mRenderTime);
}

void OgreDynamicShape::enterAnimationState(DynamicShapeState* animationState)
{
	
	if (animationState == Idle_InterpolateTick_Animation::Instance())
	{
		// start off in the idle state (top and bottom together)
		setBaseAnimation(ANIM_IDLE_BASE,false);
		setTopAnimation(ANIM_IDLE_TOP,false);

		// relax the hands since we're not holding anything
		mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
	}
	else if (animationState == Run_InterpolateTick_Animation::Instance())
	{
		setBaseAnimation(ANIM_RUN_BASE, true);
	    setTopAnimation(ANIM_RUN_TOP, true);

		// relax the hands since we're not holding anything
		if (!mAnims[ANIM_HANDS_RELAXED]->getEnabled())
		{
			mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
		}
	}
}

void OgreDynamicShape::fadeAnimations(Real deltaTime)
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

void OgreDynamicShape::setBaseAnimation(AnimID id, bool reset)
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

void OgreDynamicShape::setTopAnimation(AnimID id, bool reset)
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

//title

void OgreDynamicShape::drawTitle()
{
	mObjectTitle->setTitle(mObjectTitleString); 
	mObjectTitle->update();
}
void OgreDynamicShape::appendToTitle(std::string appendage)
{
	mObjectTitleString.append(appendage);
}

void OgreDynamicShape::appendToTitle(int appendage)
{
	mObjectTitleString.append(StringConverter::toString(appendage));
}

void OgreDynamicShape::clearTitle()
{
	mObjectTitleString.clear();
}
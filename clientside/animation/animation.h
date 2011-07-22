#ifndef OGREANIMATION_H
#define OGREANIMATION_H

#define NUM_ANIMS 13           // number of animations the character has
#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//parents
#include "../../clientside/shape/shape.h"

// all the animations our character has, and a null ID
// some of these affect separate body parts and will be blended together
enum AnimID
{
	ANIM_IDLE_BASE,
    ANIM_IDLE_TOP,
    ANIM_RUN_BASE,
    ANIM_RUN_TOP,
    ANIM_HANDS_CLOSED,
    ANIM_HANDS_RELAXED,
    ANIM_DRAW_SWORDS,
    ANIM_SLICE_VERTICAL,
    ANIM_SLICE_HORIZONTAL,
    ANIM_DANCE,
    ANIM_JUMP_START,
    ANIM_JUMP_LOOP,
    ANIM_JUMP_END,
    ANIM_NONE
};

class AnimationStateMachine;

class OgreAnimation : public virtual Shape
{

public:
	OgreAnimation();
	~OgreAnimation();

//ticks
void processTick    ();
void interpolateTick    ();

AnimationStateMachine* mAnimationProcessTickStateMachine;
AnimationStateMachine* mAnimationInterpolateTickStateMachine;

//animation
//void         updateAnimations (Real Yvelocity, Real renderTime, bool stop, float animSpeed);
void         setupAnimations();
void         fadeAnimations   (Real deltaTime);
void         setTopAnimation  (AnimID id, bool reset);
void         setBaseAnimation (AnimID id, bool reset);

//animation
Ogre::AnimationState* mAnims[NUM_ANIMS];     // master animation list
AnimID          mBaseAnimID;           // current base (full- or lower-body) animation
AnimID          mTopAnimID;            // current top (upper-body) animation
bool            mFadingIn[NUM_ANIMS];  // which animations are fading in
bool            mFadingOut[NUM_ANIMS]; // which animations are fading out
Ogre::AnimationState  *mAnimationState;

};

#endif


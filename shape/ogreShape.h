#ifndef CLIENTSIDESHAPE_H
#define CLIENTSIDESHAPE_H

#define NUM_ANIMS 13           // number of animations the character has
#define CHAR_HEIGHT 5          // height of character's center of mass above ground
#define CAM_HEIGHT 2           // height of camera above character's center of mass
#define RUN_SPEED 17           // character running speed in units per second
#define TURN_SPEED 500.0f      // character turning in degrees per second
#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second

#include "shape.h"

#include "Ogre.h"
#include "OIS.h"

using namespace Ogre;

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

class OgreShape : public Shape
{

public:
OgreShape(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~OgreShape();

SceneNode* getSceneNode() { return mSceneNode; }

//mesh
std::string mMeshName;

Entity* mEntity;

//scene
Ogre::SceneManager* mSceneManager;
SceneNode*          mSceneNode;

//animation
void         updateAnimations (Real renderTime, bool stop);
void         setupAnimations  ();
void         fadeAnimations   (Real deltaTime);
void         setTopAnimation  (AnimID id, bool reset);
void         setBaseAnimation (AnimID id, bool reset);
void         addTime          (Real deltaTime);

//animation
AnimationState* mAnims[NUM_ANIMS];     // master animation list
AnimID          mBaseAnimID;           // current base (full- or lower-body) animation
AnimID          mTopAnimID;            // current top (upper-body) animation
bool            mFadingIn[NUM_ANIMS];  // which animations are fading in
bool            mFadingOut[NUM_ANIMS]; // which animations are fading out
Real            mTimer;                // general timer to see how long animations have been playing
AnimationState  *mAnimationState;
bool            mStop;

};

#endif


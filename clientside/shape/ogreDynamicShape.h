#ifndef OGREDYNAMICSHAPE_H
#define OGREDYNAMICSHAPE_H

#define NUM_ANIMS 13           // number of animations the character has
#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second

#include "dynamicShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class ObjectTitle;

#include <string>

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


class OgreDynamicShape : public DynamicShape //, public OgreAnimation
{

public:
OgreDynamicShape(Game* game, Dispatch* dispatch, bool isGhost);
~OgreDynamicShape();


//ogre scene stuff
std::string         mMeshName;
std::string         mName;
Entity*             mEntity;

//this is your pointer to move shape, really all you need.
SceneNode*          mSceneNode;

//animation
Ogre::AnimationState* mAnims[NUM_ANIMS];     // master animation list
AnimID                mBaseAnimID;           // current base (full- or lower-body) animation
AnimID                mTopAnimID;            // current top (upper-body) animation
bool                  mFadingIn[NUM_ANIMS];  // which animations are fading in
bool                  mFadingOut[NUM_ANIMS]; // which animations are fading out
Ogre::AnimationState  *mAnimationState;

//billboard
ObjectTitle* mObjectTitle;
std::string  mObjectTitleString;

//shape
void createShape();

//ghost
bool mIsGhost;

//debugging
void checkExtents(Vector3D min);

//size
void scale(Vector3D scaleVector);

//movement
void yaw        (float amountToYaw, bool converToDegree   );
void translate  (Vector3D translateVector, int perspective);
void setPosition(Vector3D position                        );
void setPosition(float x, float y, float z                );
void setVisible (bool visible                             );

//check position
float    getDegreesToSomething(Vector3D something                       );
Vector3D getPosition          (                                         );

//animation
void runAnimations();
void enterAnimationState(DynamicShapeState* animationState);
void fadeAnimations   (Real deltaTime);
void setTopAnimation  (AnimID id, bool reset);
void setBaseAnimation (AnimID id, bool reset);
void setupAnimations();

//title
void drawTitle    ();
void appendToTitle(std::string appendage);
void appendToTitle(int appendage);
void clearTitle   ();
void setupTitle();

//name
std::string getName() { return mName; } 

//ogre scene node
SceneNode*  getSceneNode() { return mSceneNode; }

//utility
Ogre::Vector3 converToVector3(Vector3D vector3d);



};

#endif


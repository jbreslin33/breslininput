#ifndef DYNAMICSHAPE_H
#define DYNAMICSHAPE_H

//parents
#include "../../clientside/move/move.h"
#include "../../clientside/rotation/rotation.h"
#include "../../clientside/animation/animation.h"

#include <string>

//i want a dynamic shape...this should be dynamic shape shape should hide ogreshape...

class DynamicShape : public Move , public Rotation, public OgreAnimation
{

public:
DynamicShape(int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::SceneManager* mSceneMgr, std::string mesh);
~DynamicShape();

//ticks
void processTick();
void interpolateTick(float renderTime);

};

#endif


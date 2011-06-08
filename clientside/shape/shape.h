#ifndef SHAPE_H
#define SHAPE_H

//parents
#include "../../clientside/move/move.h"
#include "../../clientside/rotation/rotation.h"
#include "../../animation/ogreAnimation.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

#include <string>

class Shape : public Move , public Rotation, public OgreAnimation
{

public:
Shape(Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~Shape();

//ticks
void processTick();
void interpolateTick(float renderTime);

};

#endif


#ifndef SHAPE_H
#define SHAPE_H

//parents
#include "../../clientside/move/move.h"
#include "../../clientside/rotation/rotation.h"
#include "../../clientside/animation/animation.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

#include <string>

class Shape : public Move , public Rotation, public Animation
{

public:
Shape(int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::SceneManager* mSceneMgr, std::string mesh);
~Shape();

//ticks
void processTick();
void interpolateTick(float renderTime);

};

#endif


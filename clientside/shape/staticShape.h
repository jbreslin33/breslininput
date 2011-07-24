#ifndef STATICSHAPE_H
#define STATICSHAPE_H

#include "shape.h"

#include <string>

//i want a static shape...this should be static shape shape should hide ogreshape...
class Game;
class StaticShapeStateMachine;

class StaticShape : public Shape //, public OgreAnimation
{

public:
StaticShape(Game* game,int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation);
~StaticShape();

//game pointer cause you gotta know about the world you inhabit as a playa
Game* mGame;

};

#endif


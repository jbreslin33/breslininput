#ifndef DYNAMICSHAPE_H
#define DYNAMICSHAPE_H

#include "shape.h"

#include <string>

//i want a dynamic shape...this should be dynamic shape shape should hide ogreshape...

class DynamicShape : Shape //, public OgreAnimation
{

public:
DynamicShape(Game* game, int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, std::string mesh);
~DynamicShape();

//ticks
void processTick();
void interpolateTick(float renderTime);

float getDegreesToServer();
void calculateServerRotationSpeed();

};

#endif


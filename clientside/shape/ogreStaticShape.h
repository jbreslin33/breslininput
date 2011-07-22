#ifndef DYNAMICSHAPE_H
#define DYNAMICSHAPE_H

#include "ogreShape.h"

#include <string>

class OgreDynamicShape : public DynamicShape //, public OgreAnimation
{

public:
OgreDynamicShape(Game* game, int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, std::string mesh);
~OgreDynamicShape();



};

#endif


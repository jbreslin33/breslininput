#ifndef OGRESTATICSHAPE_H
#define OGRESTATICSHAPE_H

#include "staticShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

#include <string>

class OgreStaticShape : public StaticShape //, public OgreAnimation
{

public:
OgreStaticShape(Game* game, int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, std::string mesh);
~OgreStaticShape();
};

#endif


#ifndef OGRESTATICSHAPE_H
#define OGRESTATICSHAPE_H

#include "ogreDynamicShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

#include <string>

/***
Just overide what you need to shut off movement,animation etc.
*/

class OgreStaticShape : public OgreDynamicShape //, public OgreAnimation
{

public:
OgreStaticShape(Game* game, int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, std::string mesh);
~OgreStaticShape();
};

#endif


#include "ogreStaticShape.h"
#include "../../tdreamsock/dreamSockLog.h"

#include <string.h>

OgreStaticShape::OgreStaticShape(Game* game, int ind, Vector3D* position, Vector3D* velocity,
						   Vector3D* rotation, std::string mesh)
:
	OgreDynamicShape         (game,ind,position,velocity,rotation,mesh)
{

}

OgreStaticShape::~OgreStaticShape()
{
}

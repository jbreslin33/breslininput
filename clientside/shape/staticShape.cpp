#include "staticShape.h"
#include "../../tdreamsock/dreamSockLog.h"

#include <string.h>

StaticShape::StaticShape(Game* game,int ind, Vector3D* position, Vector3D* velocity,
						   Vector3D* rotation)
:
	Shape         (ind,position,rotation)
{
	mGame = game;
}

StaticShape::~StaticShape()
{

}

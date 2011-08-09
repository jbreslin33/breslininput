#include "shape.h"
#include "../tdreamsock/dreamSockLog.h"

#include "shape.h"
#include "../tdreamsock/dreamSockLog.h"

Shape::Shape(int ind, Vector3D* position, Vector3D* rotation)
{
	mIndex = ind;
}
Shape::Shape()
{
	mIndex = 0;
}
Shape::~Shape()
{
}

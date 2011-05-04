#include "shape.h"
#include "../tdreamsock/dreamSock.h"

Shape::Shape(std::string shapeName, int xposition, int yposition, int zposition)
{
   	mShapeName = shapeName;

	//starting position
   	x = xposition;
   	y = yposition;
   	z = zposition;
}

Shape::~Shape()
{
}












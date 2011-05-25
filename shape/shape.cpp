#include "shape.h"
#include "../math/vector3D.h"

Shape::Shape(std::string shapeName, Vector3D* position)
{
   	mShapeName = shapeName;
	mPosition = position;

}

Shape::~Shape()
{
}












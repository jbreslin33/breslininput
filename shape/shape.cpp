#include "shape.h"
#include "../math/vector3D.h"

Shape::Shape()
{

}

Shape::Shape(std::string shapeName, Vector3D* position)
{
 	mPosition = position;
	mIndex  = 0;
   	mName = shapeName;
	mProcessedFrame = 0;

}

Shape::~Shape()
{
}

void Shape::processTick()
{
	//Move::processTick();
	//Rotation::processTick();
}
void Shape::interpolateTick(float renderTime)
{
	//Move::interpolateTick(renderTime);
	//Rotation::interpolateTick(renderTime);
}






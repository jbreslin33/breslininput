#ifndef SHAPE_H
#define SHAPE_H

#include <string>

class Vector3D;

class Shape
{

public:

Shape(std::string name, Vector3D* position);
~Shape();

//stats
std::string mShapeName;
Vector3D* mPosition;

};

#endif


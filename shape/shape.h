#ifndef SHAPE_H
#define SHAPE_H

#include <string>

class Shape
{

public:

Shape(std::string name, int x, int y, int z);
~Shape();

//stats
std::string mShapeName;
int x;
int y;
int z;

};

#endif


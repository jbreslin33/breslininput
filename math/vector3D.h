#ifndef VECTOR3D_H
#define VECTOR3D_H

class Vector3D
{

public:
Vector3D();
~Vector3D();

void normalize();
double length();

float x;
float y;
float z;

};
#endif

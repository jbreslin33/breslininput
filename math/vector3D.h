#ifndef VECTOR3D_H
#define VECTOR3D_H

class Vector3D
{

public:
Vector3D();
~Vector3D();

double length();

float x;
float y;
float z;

bool isZero();
void zero();

void normalise();

Vector3D operator*(float num) const;

};
#endif

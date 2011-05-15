#include "vector3D.h"
#include <math.h>

Vector3D::Vector3D()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector3D::~Vector3D()
{
}

double Vector3D::length()
{
	return sqrt(x*x + y*y + z*z);
}


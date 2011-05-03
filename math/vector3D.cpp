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

void Vector3D::normalize()
{
	// Vector3.Length property is under length section
    double length = this->length();
 
    x /= length;
    y /= length;
    z /= length;
  }

double Vector3D::length()
{
	return sqrt(x*x + y*y + z*z);
}


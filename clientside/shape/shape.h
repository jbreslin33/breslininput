#ifndef SHAPE_H
#define SHAPE_H

//utility
#include <string>

//math
#include "../../math/vector3D.h"

class Shape
{
public:

Shape(int ind, Vector3D* position, Vector3D* rotation);
~Shape();

//virtual functions..

//setting position
virtual void     setPosition          (Vector3D                  ) = 0;
virtual void     setPosition          (float x, float y, float z ) = 0;

//animation
virtual void     runAnimations        (                          ) = 0;   

//getting position
virtual Vector3D getPosition          (                          ) = 0;

//size
virtual void     scale                (Vector3D                  ) = 0;

//visibility
virtual void     setVisible           (bool b                    ) = 0;

//title
virtual void     drawTitle            (                          ) = 0;
virtual void     appendToTitle        (std::string appendage     ) = 0;
virtual void     appendToTitle        (int appendage             ) = 0;
virtual void     clearTitle           (                          ) = 0;

//id
int			 mIndex;

};

#endif


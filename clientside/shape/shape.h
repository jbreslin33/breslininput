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

//virtual
virtual void     setPosition          (Vector3D                  ) = 0;
virtual void     setPosition          (float x, float y, float z ) = 0;
virtual Vector3D getPosition          (                          ) = 0;
virtual void     scale                (Vector3D                  ) = 0;
virtual void     setVisible           (bool b                    ) = 0;
virtual void     drawTitle            (std::string title         ) = 0;
virtual void     appendToTile         (std::string appendage     ) = 0;

//id
int			 mIndex;

};

#endif


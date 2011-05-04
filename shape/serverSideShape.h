#ifndef SERVERSIDESHAPE_H
#define SERVERSIDESHAPE_H

#include "shape.h"

class ServerSideShape : public Shape
{

public:

ServerSideShape(std::string name, int xPos, int yPos, int zPos); 
~ServerSideShape();


};

#endif


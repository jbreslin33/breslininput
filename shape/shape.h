#ifndef SHAPE_H
#define SHAPE_H

#include <string>
#define COMMAND_HISTORY_SIZE		64

#include "../command/command.h"

class Vector3D;

class Shape
{

public:

	Shape();
Shape(std::string name, Vector3D* position);
~Shape();

//stats
Vector3D* mPosition;

//ticks
void processTick    ();
void interpolateTick    (float renderTime);

//id
int			mIndex;
std::string mName;

Command mFrame[COMMAND_HISTORY_SIZE];
Command	mServerFrame;					// the latest frame from server
Command	mCommand;						// current frame's commands

long mProcessedFrame;

};

#endif


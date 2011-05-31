#ifndef SERVERSIDECOMMAND_H
#define SERVERSIDECOMMAND_H

#include "command.h"

class ServerSideCommand : public Command
{
public:
ServerSideCommand();
~ServerSideCommand();
	
//	int key;								// Pressed keys

//	Vector3D vel;							// Velocity
//	Vector3D origin;						// Position

//	int msec;								// How long to run command (in ms)
};
#endif

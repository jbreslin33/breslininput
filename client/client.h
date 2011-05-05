#ifndef CLIENT_H
#define CLIENT_H

#define COMMAND_HISTORY_SIZE		64

#include "../command/Command.h"
#include "../math/vector3D.h"

class Client
{

public:

Client();
~Client();

Command mFrame[64];
Command	mServerFrame;					// the latest frame from server
Command	mCommand;						// current frame's commands

long mProcessedFrame;

};
#endif

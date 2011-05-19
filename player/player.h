#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#define COMMAND_HISTORY_SIZE		64

#include "../command/command.h"

#include "../tdreamsock/dreamSock.h"

class Player
{

public:

Player(std::string name);
~Player();

//id
int			mIndex;
std::string mName;

Command mFrame[64];
Command	mServerFrame;					// the latest frame from server
Command	mCommand;						// current frame's commands

long mProcessedFrame;
dreamClient* netClient;

};

#endif


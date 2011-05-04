#ifndef PLAYER_H
#define PLAYER_H

#include "../command/Command.h"

#include <string>

class Client;
class Shape;

class Player
{

public:

Player(Client* client, Shape* shape, std::string name);
~Player();

//commands
Command*    mFrame[64];
Command*	mServerFrame;					// the latest frame from server
Command*	mCommand;						// current frame's commands

//id
int			mIndex;
std::string mName;

//associated objects
Client* mClient;
Shape* mShape;

};

#endif


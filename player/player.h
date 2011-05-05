#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player
{

public:

Player(std::string name);
~Player();

//id
int			mIndex;
std::string mName;

};

#endif


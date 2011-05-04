#include "player.h"
#include "../shape/shape.h"
#include "../tdreamsock/dreamSock.h"
#include <math.h>

#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8

Player::Player(std::string name)
{
	//id
	mIndex  = 0;
   	mName = name;
}

Player::~Player()
{
}








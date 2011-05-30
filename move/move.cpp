#include "move.h"
#include "../tdreamsock/dreamSockLog.h"

//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16

Move::Move()
{
	mRunSpeed     = 0.0;
}

Move::~Move()
{
}

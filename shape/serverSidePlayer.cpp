#include "serverSidePlayer.h"
#include "../tdreamsock/dreamSock.h"
#include "../math/vector3D.h"
#include <math.h>

#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
ServerSidePlayer::ServerSidePlayer(ServerSideClient* client, std::string shapeName,int x, int y, int z) : ServerSideShape(shapeName,x,y,z)
{
	mClient = client;
}

ServerSidePlayer::~ServerSidePlayer()
{
}

void ServerSidePlayer::processTick()
{
	float clientFrametime;

	float multiplier = 100.0f;

	clientFrametime = mClient->command.msec / 1000.0f;;

	mClient->character->calculateVelocity(&mClient->command, clientFrametime);

	int f = mClient->netClient->GetIncomingSequence() & (COMMAND_HISTORY_SIZE-1);
	mClient->processedFrame = f;

}
void ServerSidePlayer::calculateVelocity(ServerSideCommand *command, float frametime)
{
	float multiplier = 100.0f;

	command->vel.x = 0.0f;
	command->vel.z = 0.0f;

	if(command->key & KEY_UP)
	{
		command->vel.z += -1;
	}

	if(command->key & KEY_DOWN)
	{
        command->vel.z += 1;
	}

	if(command->key & KEY_LEFT)
	{
		command->vel.x += -1;
	}

	if(command->key & KEY_RIGHT)
	{
		command->vel.x += 1;
	}

	float length = sqrt(pow(command->vel.x, 2) + pow(command->vel.z, 2));
	if(length != 0.0)
	{
	   command->vel.x = command->vel.x/length * 0.1 * frametime * 1000;
	   command->vel.z = command->vel.z/length * 0.1 * frametime * 1000;
	}
}

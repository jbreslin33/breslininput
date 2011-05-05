#include "serverSidePlayer.h"
#include "../tdreamsock/dreamSock.h"
#include "../math/vector3D.h"
#include <math.h>

#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
ServerSidePlayer::ServerSidePlayer(std::string name, ServerSideClient* client) : Player(name)
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

	clientFrametime = mClient->mCommand.mMilliseconds / 1000.0f;;

	calculateVelocity(&mClient->mCommand, clientFrametime);

	int f = mClient->netClient->GetIncomingSequence() & (COMMAND_HISTORY_SIZE-1);

}
void ServerSidePlayer::calculateVelocity(Command *command, float frametime)
{
	float multiplier = 100.0f;

	command->mVelocity.x = 0.0f;
	command->mVelocity.z = 0.0f;

	if(command->mKey & KEY_UP)
	{
		command->mVelocity.z += -1;
	}

	if(command->mKey & KEY_DOWN)
	{
        command->mVelocity.z += 1;
	}

	if(command->mKey & KEY_LEFT)
	{
		command->mVelocity.x += -1;
	}

	if(command->mKey & KEY_RIGHT)
	{
		command->mVelocity.x += 1;
	}

	float length = sqrt(pow(command->mVelocity.x, 2) + pow(command->mVelocity.z, 2));
	if(length != 0.0)
	{
	   command->mVelocity.x = command->mVelocity.x/length * 0.1 * frametime * 1000;
	   command->mVelocity.z = command->mVelocity.z/length * 0.1 * frametime * 1000;
	}
	LogString("calc vel.x:%f",command->mVelocity.x);
	LogString("calc vel.z:%f",command->mVelocity.z);
}

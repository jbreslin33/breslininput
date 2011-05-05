#ifndef SERVERSIDEPLAYER_H
#define SERVERSIDEPLAYER_H

#include "../player/player.h"
#include "../client/serverSideClient.h"

class ServerSidePlayer : public Player
{

public:

ServerSidePlayer(std::string name, ServerSideClient* client);
~ServerSidePlayer();

//movement
void processTick();
void calculateVelocity(Command *command, float frametime);

//associated client
ServerSideClient* mClient;


};

#endif


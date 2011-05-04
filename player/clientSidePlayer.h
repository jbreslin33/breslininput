#ifndef CLIENTSIDEPLAYER_H
#define CLIENTSIDEPLAYER_H

#include "player.h"

#include "Ogre.h"
#include "OIS.h"

using namespace Ogre;

class ClientSideClient;
class ClientSideShape;
class Command;

class ClientSidePlayer : public Player
{

public:

ClientSidePlayer(std::string name, ClientSideClient* client, ClientSideShape* shape); 
~ClientSidePlayer();

//movement
void ProcessTick();
void interpolateTick(float renderTime);
void CalculateVelocity(Command *command, float frametime);

//associated client
ClientSideClient* mClient;

//associated shape
ClientSideShape* mShape;

};

#endif


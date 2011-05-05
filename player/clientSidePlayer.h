#ifndef CLIENTSIDEPLAYER_H
#define CLIENTSIDEPLAYER_H

#include "player.h"

class ClientSideClient;
class OgreAnimatedShape;
class Command;

class ClientSidePlayer : public Player
{

public:

ClientSidePlayer(std::string name, ClientSideClient* client, OgreAnimatedShape* shape);
~ClientSidePlayer();

//movement
void processTick();
void interpolateTick(float renderTime);
void calculateVelocity(Command *command, float frametime);

//associated client
ClientSideClient* mClient;

//associated Shape
OgreAnimatedShape* mShape;

};

#endif


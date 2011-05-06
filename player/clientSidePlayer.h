#ifndef CLIENTSIDEPLAYER_H
#define CLIENTSIDEPLAYER_H

#include "player.h"

#define TURN_SPEED 500.0f      // character turning in degrees per second

class ClientSideClient;
class OgreShape;
class Command;

class ClientSidePlayer : public Player
{

public:

ClientSidePlayer(std::string name, ClientSideClient* client, OgreShape* shape);
~ClientSidePlayer();

//movement
void processTick    ();
void processRotation();

void interpolateTick    (float renderTime);
void interpolateRotation(float renderTime);


void calculateVelocity(Command *command, float frametime);

//associated client
ClientSideClient* mClient;

//associated Shape
OgreShape* mShape;

};

#endif


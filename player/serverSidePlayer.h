#ifndef SERVERSIDEPLAYER_H
#define SERVERSIDEPLAYER_H

#include "../player/player.h"
#include "../client/serverSideClient.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class OgreShape;

class ServerSidePlayer : public Player
{

public:

ServerSidePlayer(std::string name, ServerSideClient* client, OgreShape* shape);
~ServerSidePlayer();

//movement
void processTick();
void calculateVelocity(Command *command, float frametime);
void setKeyDirection();

//associated client
ServerSideClient* mClient;

//associated shape
OgreShape* mShape;

//keys
Vector3 mKeyDirection;

Vector3 mGoalDirection;

};

#endif


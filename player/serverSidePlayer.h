#ifndef SERVERSIDEPLAYER_H
#define SERVERSIDEPLAYER_H

#include "../player/player.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class OgreShape;
class Client;

class ServerSidePlayer : public Player
{

public:

ServerSidePlayer(std::string name, Client* client, OgreShape* shape);
~ServerSidePlayer();

//movement
void processTick();
void calculateVelocity(Command *command, float frametime);
void setKeyDirection();
void startJump();

//associated client
Client* mClient;

//associated shape
OgreShape* mShape;

//keys
Vector3 mKeyDirection;

Vector3 mGoalDirection;


bool mJumping;

float mRunSpeed;
float mRunAccel;
float mRunDecel;
float mVerticalVelocity;
float mJumpAccel;
float mGravity;

};

#endif


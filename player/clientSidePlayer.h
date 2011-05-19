#ifndef CLIENTSIDEPLAYER_H
#define CLIENTSIDEPLAYER_H

#include "player.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class ClientSideClient;
class OgreShape;
class Move;
class Rotation;

class ClientSidePlayer : public Player
{

public:

ClientSidePlayer(std::string name, ClientSideClient* client, OgreShape* shape);
~ClientSidePlayer();

//ticks
void processTick    ();
void interpolateTick    (float renderTime);

//associated client
ClientSideClient* mClient;

//associated Shape
OgreShape* mShape;

//Move 
Move* mMove;

//Rotation
Rotation* mRotation;

int			mIndex;

};

#endif


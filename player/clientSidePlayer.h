#ifndef CLIENTSIDEPLAYER_H
#define CLIENTSIDEPLAYER_H

#include "player.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class OgreShape;
class Move;
class Rotation;

class ClientSidePlayer : public Player
{

public:

ClientSidePlayer(std::string name, OgreShape* shape);
~ClientSidePlayer();

//ticks
void processTick    ();
void interpolateTick    (float renderTime);

//associated Shape
OgreShape* mShape;

//Move 
Move* mMove;

//Rotation
Rotation* mRotation;

int			mIndex;

ClientSidePlayer* mClientSideServerPlayer;

};

#endif


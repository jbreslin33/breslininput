#ifndef CLIENTSIDEPLAYER_H
#define CLIENTSIDEPLAYER_H

#include "player.h"
#include "../move/move.h"
#include "../rotation/rotation.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class OgreShape;

class ClientSidePlayer : public Player, public Move, public Rotation
{

public:

ClientSidePlayer(std::string name, OgreShape* shape);
~ClientSidePlayer();

//ticks
void processTick    ();
void interpolateTick    (float renderTime);

//associated Shape
OgreShape* mShape;

int			mIndex;

ClientSidePlayer* mClientSideServerPlayer;

};

#endif


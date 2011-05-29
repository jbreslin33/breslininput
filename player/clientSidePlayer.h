#ifndef CLIENTSIDEPLAYER_H
#define CLIENTSIDEPLAYER_H

#include "player.h"
#include "../move/move.h"
#include "../rotation/rotation.h"
#include "../shape/clientSideShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Client;

class ClientSidePlayer : public Player, public Move, public Rotation, public ClientSideShape
{

public:

ClientSidePlayer(Client* client, std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~ClientSidePlayer();

//ticks
void processTick    ();
void interpolateTick    (float renderTime);

Client* mClient;

int			mIndex;

ClientSidePlayer* mServerPlayer;


};

#endif


#ifndef CLIENTSIDEPLAYER_H
#define CLIENTSIDEPLAYER_H

#include "player.h"

#include "../move/clientSideMove.h"
#include "../rotation/clientSideRotation.h"
#include "../shape/clientSideShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class ClientSidePlayer : public Player, public ClientSideMove, public ClientSideRotation 
{

public:

ClientSidePlayer(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~ClientSidePlayer();




int			mIndex;

ClientSidePlayer* mServerPlayer;


};

#endif


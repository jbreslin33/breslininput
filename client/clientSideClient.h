#ifndef CLIENTSIDECLIENT_H
#define CLIENTSIDECLIENT_H

#include "client.h"

#include "Ogre.h"

#include "../tdreamsock/dreamSock.h"
#include "../command/clientSideCommand.h"
//#include "../shape/clientSideShape.h"

using namespace Ogre;

class DreamClient;
class ClientSidePlayer;

class ClientSideClient : public Client
{

public:
ClientSideClient();
~ClientSideClient();
	ClientSideCommand	frame[64];	// frame history
	ClientSideCommand	serverFrame;					// the latest frame from server
	ClientSideCommand	command;						// current frame's commands

	ClientSidePlayer* mPlayer;
	ClientSidePlayer* mServerPlayer;

	int			mIndex;
};

#endif

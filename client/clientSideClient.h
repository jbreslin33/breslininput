#ifndef CLIENTSIDECLIENT_H
#define CLIENTSIDECLIENT_H

#include "client.h"

class ClientSidePlayer;

class ClientSideClient : public Client
{

public:
ClientSideClient();
~ClientSideClient();

ClientSidePlayer* mPlayer;
ClientSidePlayer* mServerPlayer;

int			mIndex;
};

#endif

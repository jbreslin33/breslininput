#ifndef SERVERSIDECLIENT_H
#define SERVERSIDECLIENT_H

#include "client.h"

#include "../tdreamsock/dreamSock.h"
#include "../command/serverSideCommand.h"

class DreamClient;
class ServerSidePlayer;

class ServerSideClient : public Client
{

public:
ServerSideClient();
~ServerSideClient();

struct sockaddr  address;

dreamClient      *netClient;

ServerSidePlayer* mPlayer;
};

#endif

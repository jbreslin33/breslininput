#ifndef SERVER_H
#define SERVER_H

#include "../message/message.h"

#include <vector>

// Connection states
#define DREAMSOCK_CONNECTING			0
#define DREAMSOCK_CONNECTED				1
#define DREAMSOCK_DISCONNECTING			2
#define DREAMSOCK_DISCONNECTED			4

#ifdef WIN32
	#define DREAMSOCK_INVALID_SOCKET	INVALID_SOCKET
#else
	#define DREAMSOCK_INVALID_SOCKET	-1
#endif

// System messages
// Note (for all messages - system and user):
// positive = sequenced message
// negative = un-sequenced message
#define DREAMSOCK_MES_CONNECT		-101
#define DREAMSOCK_MES_DISCONNECT	-102
#define DREAMSOCK_MES_ADDSHAPE		-103
#define DREAMSOCK_MES_REMOVESHAPE	-104
#define DREAMSOCK_MES_PING			-105

// Introduce classes
class Network;
class Client;
class Game;
class Shape;

class Server
{
public:
	void			sendAddAIShape(Shape* shape);
	void			sendAddShape(Client* client);
	void			sendRemoveShape(Shape* shape);
	void			addClient(struct sockaddr *address, char *name);

	void			removeClient(Client *client);

	
	void			parsePacket(Message *mes, struct sockaddr *address);
	int				checkForTimeout(char *data, struct sockaddr *from);

	std::vector<Client*> mClientVector;

	int				port;					// Port

	bool			init;

public:
					Server(Game* serverSideGame,const char *localIP, int serverPort);
					~Server();

	int				getPacket(char *data, struct sockaddr *from);
	void			sendPackets(void);

	int				getPort(void)			{ return port; }

	Game* mGame;
	Network* mNetwork;

	void	readPackets(void);
	const char *mLocalIP;
};

#endif

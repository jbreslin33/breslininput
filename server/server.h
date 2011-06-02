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
#define DREAMSOCK_MES_ADDCLIENT		-103
#define DREAMSOCK_MES_REMOVECLIENT	-104
#define DREAMSOCK_MES_PING			-105

// Introduce classes
class Network;
class Client;
class Game;

class Server
{
public:
	void			SendAddClient(Client *newClient);
	void			SendRemoveClient(Client *client);
	void			AddClient(struct sockaddr *address, char *name);
	void			RemoveClient(Client *client);
	void			ParsePacket(Message *mes, struct sockaddr *address);
	int				CheckForTimeout(char *data, struct sockaddr *from);

	std::vector<Client*> mClientVector;

	int				port;					// Port

	int				runningIndex;			// Running index numbers for new clients

	bool			init;

public:
					Server(Game* serverSideGame,const char *localIP, int serverPort);
					~Server();

	void			Uninitialise(void);
	void			SendPing(void);
	int				GetPacket(char *data, struct sockaddr *from);
	void			SendPackets(void);

	int				GetPort(void)			{ return port; }

	Game* mGame;
	Network* mNetwork;

	void	ReadPackets(void);
	const char *mLocalIP;
};

#endif

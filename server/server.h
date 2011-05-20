#ifndef SERVER_H
#define SERVER_H

#include "../tdreamsock/dreamSockLog.h"
#include "../message/message.h"


#include <vector>

#ifdef WIN32
	#pragma comment (lib,"ws2_32.lib")
	#pragma message ("Auto linking WinSock2 library")

	#include <winsock2.h>
#else
	#include <string.h>
	#include <netinet/in.h>
#endif

#include <stdio.h>
#include <stddef.h>

// Define SOCKET data type for UNIX (defined in WinSock for Win32)
// And socklen_t for Win32
#ifdef WIN32
	typedef int socklen_t;
#else
	typedef int SOCKET;

	#ifndef TRUE
	#define TRUE 1
	#endif
	#ifndef FALSE
	#define FALSE 0
	#endif
#endif

// Host types
#define DREAMSERVER						-1
#define DREAMCLIENT						0

// Connection protocols
#define DREAMSOCK_TCP					0
#define DREAMSOCK_UDP					1

// Connection states
#define DREAMSOCK_CONNECTING			0
#define DREAMSOCK_CONNECTED				1
#define DREAMSOCK_DISCONNECTING			2
#define DREAMSOCK_DISCONNECTED			4

// Error codes
#define DREAMSOCK_SERVER_ERROR			1
#define DREAMSOCK_CLIENT_ERROR			2

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
class dreamClient;
class dreamServer;
class dreamSock;
class ServerSideGame;

class Server
{
public:
	void			SendAddClient(dreamClient *newClient);
	void			SendRemoveClient(dreamClient *client);
	void			AddClient(struct sockaddr *address, char *name);
	void			RemoveClient(dreamClient *client);
	void			ParsePacket(Message *mes, struct sockaddr *address);
	int				CheckForTimeout(char *data, struct sockaddr *from);

	std::vector<dreamClient*> mClientVector;

	int				port;					// Port
	SOCKET			socket;					// Socket
	int				runningIndex;			// Running index numbers for new clients

	bool			init;

public:
					Server(ServerSideGame* serverSideGame);
					~Server();

	int				Initialise(const char *localIP, int serverPort);
	void			Uninitialise(void);
	void			SendPing(void);
	int				GetPacket(char *data, struct sockaddr *from);
	void			SendPackets(void);

	bool			GetInit(void)			{ return init; }

	int				GetPort(void)			{ return port; }

	ServerSideGame* mServerSideGame;
};

#endif

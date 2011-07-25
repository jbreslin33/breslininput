#ifndef NETWORK_H
#define NETWORK_H

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

// Connection protocols
#define DREAMSOCK_TCP					0
#define DREAMSOCK_UDP					1

#ifdef WIN32
	#define DREAMSOCK_INVALID_SOCKET	INVALID_SOCKET
#else
	#define DREAMSOCK_INVALID_SOCKET	-1
#endif

#ifdef WIN32
class DreamWinSock;
#else
class DreamLinuxSock;
#endif

class Client;

class Network 
{
public:
Network();
Network(Client* client,const char netInterface[32], int port);
Network(const char netInterface[32], int port);
~Network();

#ifdef WIN32
DreamWinSock* mDreamWinSock;
#else
DreamLinuxSock* mDreamLinuxSock;
#endif

// Function prototypes

void dreamSock_Shutdown(void);

SOCKET dreamSock_Socket(int protocol);
int dreamSock_SetNonBlocking(SOCKET sock, u_long setMode);
int dreamSock_SetBroadcasting(SOCKET sock, int mode);
int dreamSock_StringToSockaddr(const char *addressString, struct sockaddr *sadr);
SOCKET dreamSock_OpenUDPSocket(const char netInterface[32], int port);
void dreamSock_CloseSocket(SOCKET sock);

int dreamSock_GetPacket(SOCKET sock, char *data, struct sockaddr *from);
void dreamSock_SendPacket(SOCKET sock, int length, char *data, struct sockaddr addr);
void dreamSock_Broadcast(SOCKET sock, int length, char *data, int port);

#ifndef WIN32
int dreamSock_Linux_GetCurrentSystemTime(void);
#endif

int dreamSock_GetCurrentSystemTime(void);

SOCKET mSocket;

Client* mClient;
};
#endif

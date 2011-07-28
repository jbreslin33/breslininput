#ifndef DATAGRAMSOCKET_H
#define DATAGRAMSOCKET_H

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

class Message;

class DatagramSocket 
{
public:
DatagramSocket(const char serverIP[32], int serverPort);
DatagramSocket();
~DatagramSocket();

SOCKET mSocket;
struct sockaddr_in sendToAddress;

// Function prototypes
SOCKET dreamSock_Socket(int protocol);
int dreamSock_SetNonBlocking(u_long setMode);
SOCKET dreamSock_OpenUDPSocket();
void dreamSock_CloseSocket();

int dreamSock_GetPacket(char *data);
void dreamSock_SendPacket(int length, char *data, struct sockaddr addr);

void sendPacket(Message *theMes);

void setSendToAddress(const char* serverIP, int serverPort);

};
#endif
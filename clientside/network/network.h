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

class Dispatch;

class Network
{
public:
Network(const char serverIP[32], int serverPort);
~Network();

//send
void send(Dispatch* dispatch);

//receive
int  getPacket(Dispatch* dispatch);

//close
void close();

private:

#ifdef WIN32
DreamWinSock* mDreamWinSock;
#else
DreamLinuxSock* mDreamLinuxSock;
#endif

//socket
SOCKET mSocket;

//address
struct sockaddr_in sendToAddress;

//create
SOCKET createSocket(int protocol);
void setSendToAddress(const char* serverIP, int serverPort);

//open
SOCKET open();

//block
int    setNonBlocking(u_long setMode);



//send
void send			 (int length, char *data, struct sockaddr addr);

//parse
public:
void parsePacket(Dispatch *mes);
	unsigned short	mIncomingSequence;		// Incoming packet sequence
		unsigned short	mDroppedPackets;			// Dropped packets
};
#endif
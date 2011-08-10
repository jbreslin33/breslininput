#ifndef CLIENT_H
#define CLIENT_H

#include "../message/message.h"

//command
#include "../../command/command.h"

#ifdef WIN32
	#pragma comment (lib,"ws2_32.lib")
#pragma message ("Auto linking WinSock2 library")

	#include <winsock2.h>
#else
	#include <string.h>
	#include <netinet/in.h>
#endif

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

// Error codes
#define DREAMSOCK_SERVER_ERROR			1
#define DREAMSOCK_CLIENT_ERROR			2

// Introduce classes
class Network;
class Shape;

class Client
{
public:
	Client(Network* network);

~Client();
	int				mConnectionState;		// Connecting, connected, disconnecting, disconnected

	unsigned short	mOutgoingSequence;		// OutFgoing packet sequence
	unsigned short	mIncomingSequence;		// Incoming packet sequence
	unsigned short	mDroppedPackets;			// Dropped packets

	struct sockaddr	mMyaddress;				// Socket address

	int				mLastMessageTime;

public:

	void			SendPacket(Message *message);

	//clients address to send back messages to
	struct sockaddr *GetSocketAddress(void) { return &mMyaddress; }
	void			SetSocketAddress(struct sockaddr *address) { memcpy(&mMyaddress, address, sizeof(struct sockaddr)); }

	Message	mMessage;

    Shape* mShape;  //on server: everybody's got one ...same on clientside mShape is the clients personal avatar..
	Command mClientCommandToServerArray[64];
	Command	mClientCommandToServer; //for the human moves to be sent off to server		
	Network* mNetwork;

};
#endif

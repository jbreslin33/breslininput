#ifndef CLIENT_H
#define CLIENT_H

#include "../../message/message.h"

//command
#include "../../command/command.h"
/*
#ifdef WIN32
	#pragma comment (lib,"ws2_32.lib")
#pragma message ("Auto linking WinSock2 library")

	#include <winsock2.h>
#else
	#include <string.h>
	#include <netinet/in.h>
#endif
*/
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
class DynamicShape;

class Client
{
public:

Client(const char *localIP, const char *remoteIP, int port);
~Client();
	void			DumpBuffer(void);
	void			ParsePacket(Message *mes);
	void            Reset();
	int				mConnectionState;		// Connecting, connected, disconnecting, disconnected

	unsigned short	mOutgoingSequence;		// OutFgoing packet sequence
	unsigned short	mIncomingSequence;		// Incoming packet sequence
	unsigned short	mIncomingAcknowledged;	// Last packet acknowledged by other end
	unsigned short	mDroppedPackets;			// Dropped packets

	int				mServerPort;				// Port

	int				mLastMessageTime;

public:

	void			SendConnect(const char *name);
	void			SendDisconnect(void);

	int				GetPacket(char *data);

	void			SendPacket(Message *message);

	Message	mMessage;

    DynamicShape* mShape;  //on server: everybody's got one ...same on clientside mShape is the clients personal avatar..
	Command mClientCommandToServerArray[64];
	Command	mClientCommandToServer; //for the human moves to be sent off to server		
	Network* mNetwork;

};
#endif

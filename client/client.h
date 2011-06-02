#ifndef CLIENT_H
#define CLIENT_H

#include "../message/message.h"

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
#define DREAMSOCK_MES_ADDCLIENT		-103
#define DREAMSOCK_MES_REMOVECLIENT	-104
#define DREAMSOCK_MES_PING			-105

// Error codes
#define DREAMSOCK_SERVER_ERROR			1
#define DREAMSOCK_CLIENT_ERROR			2

// Introduce classes
class Network;
class Shape;
class Shape;

class Client
{
public:
	Client(Network* network);
	Client(const char *localIP, const char *remoteIP, int port);
~Client();
	void			DumpBuffer(void);
	void			ParsePacket(Message *mes);

	int				connectionState;		// Connecting, connected, disconnecting, disconnected

	unsigned short	outgoingSequence;		// Outgoing packet sequence
	unsigned short	incomingSequence;		// Incoming packet sequence
	unsigned short	incomingAcknowledged;	// Last packet acknowledged by other end
	unsigned short	droppedPackets;			// Dropped packets

	int				serverPort;				// Port
	char			serverIP[32];			// IP address
	int				index;					// Client index (starts from 1, running number)
	char			name[32];				// Client name


	struct sockaddr	myaddress;				// Socket address

	int				pingSent;				// When did we send ping?
	int				ping;					// Network latency

	int				lastMessageTime;

public:

	int				Initialise(const char *localIP, const char *remoteIP, int port);
	void			Uninitialise(void);
	void			Reset(void);
	void			SendConnect(const char *name);
	void			SendDisconnect(void);
	void			SendPing(void);

	void			SetConnectionState(int con)		{ connectionState = con; }
	int				GetConnectionState(void)		{ return connectionState; }

	int				GetPacket(char *data, struct sockaddr *from);
	void			SendPacket(void);
	void			SendPacket(Message *message);

	unsigned short	GetOutgoingSequence(void)				{ return outgoingSequence; }
	void			SetOutgoingSequence(unsigned short seq)	{ outgoingSequence = seq; }
	void			IncreaseOutgoingSequence(void)			{ outgoingSequence++; }
	unsigned short	GetIncomingSequence(void)				{ return incomingSequence; }
	void			SetIncomingSequence(unsigned short seq)	{ incomingSequence = seq; }
	unsigned short	GetIncomingAcknowledged(void)			{ return incomingAcknowledged; }
	void			SetIncomingAcknowledged(unsigned short seq) { incomingAcknowledged = seq; }
	unsigned short	GetDroppedPackets(void)					{ return droppedPackets; }
	void			SetDroppedPackets(unsigned short drop)	{ droppedPackets = drop; }

	int				GetIndex(void)			{ return index; }
	void			SetIndex(int ind)		{ index = ind; }

	char			*GetName(void)			{ return name; }

#ifdef WIN32
	void			SetName(char *n)		{ strcpy_s(name, n); }
#else
	void			SetName(char *n)		{ strcpy(name, n); }
#endif

	struct sockaddr *GetSocketAddress(void) { return &myaddress; }
	void			SetSocketAddress(struct sockaddr *address) { memcpy(&myaddress, address, sizeof(struct sockaddr)); }

	int				GetPingSent(void)		{ return pingSent; }
	void			SetPing(int p)			{ ping = p; }

	int				GetLastMessageTime(void) { return lastMessageTime; }
	void			SetLastMessageTime(int t) { lastMessageTime = t; }

	Message	mMessage;

Shape* mShape;

Network* mNetwork;

};
#endif

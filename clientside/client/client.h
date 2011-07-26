#ifndef CLIENT_H
#define CLIENT_H

#include "../../message/message.h"

//command
#include "../../command/command.h"

#ifdef WIN32
	#define DREAMSOCK_INVALID_SOCKET	INVALID_SOCKET
#else
	#define DREAMSOCK_INVALID_SOCKET	-1
#endif

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

	Message	mMessage;

	 //messageConnect;
	//int 
	
	static const char mMessageConnect = -101;
	static const char mMessageDisconnect = -102;
	static const char mMessageAddShape = -103;
	static const char mMessageRemoveShape = -104;

	static const char mMessageConnecting    = 0;
	static const char mMessageConnected     = 1;
	static const char mMessageDisconnecting = 2;
	static const char mMessageDisconnected  = 4;


    DynamicShape* mShape;  //on server: everybody's got one ...same on clientside mShape is the clients personal avatar..
	Command mClientCommandToServerArray[64];
	Command	mClientCommandToServer; //for the human moves to be sent off to server		
	Network* mNetwork;
	int				mServerPort;				// Port
	int				mLastMessageTime;

	void			DumpBuffer(void);
	void			ParsePacket(Message *mes);
	void            Reset();
	int				mConnectionState;		// Connecting, connected, disconnecting, disconnected

	unsigned short	mOutgoingSequence;		// OutFgoing packet sequence
	unsigned short	mIncomingSequence;		// Incoming packet sequence
	unsigned short	mIncomingAcknowledged;	// Last packet acknowledged by other end
	unsigned short	mDroppedPackets;			// Dropped packets

	void			SendConnect(const char *name);
	void			SendDisconnect(void);

	int				GetPacket(char *data);

	void			SendPacket(Message *message);

};
#endif

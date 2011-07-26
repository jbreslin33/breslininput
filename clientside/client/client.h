#ifndef CLIENT_H
#define CLIENT_H

#include "../../message/message.h"

//command
#include "../../command/command.h"

// Introduce classes
class Network;
class DynamicShape;

class Client
{
public:

Client(const char *localIP, const char *remoteIP, int port);
~Client();

	Message	mMessage;

	//used to direct messages to correct functions, the message header,
	//not to confused with packet header which deals with with networking.
	static const char mMessageConnect     = -101;
	static const char mMessageDisconnect  = -102;
	static const char mMessageAddShape    = -103;
	static const char mMessageRemoveShape = -104;

	static const char mMessageConnecting    = 0;
	static const char mMessageConnected     = 1;
	static const char mMessageDisconnecting = 2;
	static const char mMessageDisconnected  = 4;

	//the avatar
    DynamicShape* mShape;  //on server: everybody's got one ...same on clientside mShape is the clients personal avatar..
	
	//handles telegraphing key presses etc to server for your client/avatar pair. not to be
	//confused with Commands that come from server that deal with all dynamic shapes on this
	//client machine.
	Command mClientCommandToServerArray[64];
	Command	mClientCommandToServer; //for the human moves to be sent off to server

	//allows communication to server, sending and recieving packtes
	Network* mNetwork;
	int				mServerPort;				// Port
	unsigned short	mOutgoingSequence;		// OutFgoing packet sequence
	unsigned short	mIncomingSequence;		// Incoming packet sequence
	unsigned short	mIncomingAcknowledged;	// Last packet acknowledged by other end
	unsigned short	mDroppedPackets;			// Dropped packets
	int				mConnectionState;		// Connecting, connected, disconnecting, disconnected

	//packets
	int				GetPacket(char *data);
	void			SendPacket(Message *message);
	void			DumpBuffer(void);
	void			ParsePacket(Message *mes);
	void            Reset();

	//connection. why it needs it's own method i don't know i'll look into it
	void			SendConnect(const char *name);
	void			SendDisconnect(void);
};
#endif

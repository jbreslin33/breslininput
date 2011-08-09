#ifndef CLIENT_H
#define CLIENT_H

//command
#include "../../command/command.h"

// Introduce classes
class Network;
class DynamicShape;
class Dispatch;

class Client
{
public:

Client(const char *localIP, const char *remoteIP, int port);
~Client();

public:
	int mSizeOfDispatch;

private:
	//commands
	int mCommandHistorySize;

	//messages
	static const char mMessageFrame = 1;

	//key
	static const char mCommandKey          = 1;

	//milli
	static const char mCommandMilliseconds = 2;


	//used to direct messages to correct functions, the message header,
	//not to confused with packet header which deals with with networking.
	static const char mMessageConnect     = -101;
	static const char mMessageDisconnect  = -102;
public:
	static const char mMessageAddShape    = -103;

	static const char mMessageRemoveShape = -104;

	static const char mMessageConnecting    = 0;
	static const char mMessageConnected     = 1;
	static const char mMessageDisconnecting = 2;
	static const char mMessageDisconnected  = 4;

public:
	//the avatar
    DynamicShape* mShape;  //on server: everybody's got one ...same on clientside mShape is the clients personal avatar..
private:
	//handles telegraphing key presses etc to server for your client/avatar pair. not to be
	//confused with Commands that come from server that deal with all dynamic shapes on this
	//client machine.
	Command mClientCommandToServerArray[64];
public:
	Command	mClientCommandToServer; //for the human moves to be sent off to server

	//allows communication to server, sending and recieving packtes
private:
	Network*     mNetwork;
	int			 mServerPort;				// Port
	const char*  mServerIP;

	unsigned short	mOutgoingSequence;		// OutFgoing packet sequence
	unsigned short	mIncomingSequence;		// Incoming packet sequence
	unsigned short	mIncomingAcknowledged;	// Last packet acknowledged by other end
	unsigned short	mDroppedPackets;			// Dropped packets
	int				mConnectionState;		// Connecting, connected, disconnecting, disconnected


	//packets
public:
	int				getPacket  (Dispatch* dispatch);
private:
	void			sendPacket (Dispatch* dispatch);
	void			parsePacket(Dispatch* dispatch);
	void            reset();

public:
	//connection. why it needs it's own method i don't know i'll look into it
	void			sendConnect(const char *name);

	void			sendDisconnect(void);

	//commands
	void	sendCommand             ();
private:
	void	buildDeltaMoveCommand   (Dispatch* dispatch);



};
#endif

#include "client.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../network/network.h"
#include "../dispatch/dispatch.h"

//client side client constructor, one on each client machine, i.e. one instance per machine.
Client::Client(const char *localIP, const char *remoteIP, int serverPort)
{
	//commmands
	mCommandHistorySize = 64;

	mOutgoingSequence		= 1;
	mIncomingSequence		= 0;

	// Save server's address information for later use
	mServerIP = remoteIP;
	mServerPort = serverPort;

	LogString("Server's information: IP address: %s, port: %d", remoteIP, mServerPort);

	// Create client socket
	mNetwork = new Network(remoteIP,serverPort);
}

Client::~Client()
{
	mNetwork->close();
	delete mNetwork;
}
/**********  SENDS  ****/
void Client::sendConnect(const char *name)
{
	Dispatch* dispatch = new Dispatch();
	dispatch->WriteByte(mMessageConnect);
	dispatch->WriteString(name);

	sendPacket(dispatch);
}

void Client::sendDisconnect(void)
{
	Dispatch* dispatch = new Dispatch();

	dispatch->WriteByte(mMessageDisconnect);

	sendPacket(dispatch);
	reset();
}

void Client::reset(void)
{
    mOutgoingSequence                = 1;
    mIncomingSequence                = 0;
}

void Client::sendPacket(Dispatch *dispatch)
{
    //DatagramPacket* packet = new DatagramPacket(dispatch->mCharArray,dispatch->GetSize(),mServerIP,mServerPort);
	
	//mNetwork->send(packet);
	mNetwork->send(dispatch);

	dispatch->BeginReading();
	int type = dispatch->ReadByte();

	if(type > 0)
	{
		mOutgoingSequence++;
	}
}

/**********  GETS ****/
int Client::getPacket(Dispatch* dispatch)
{
	// Check if the client is set up or if it is disconnecting
	if(!mNetwork)
	{
		return 0;
	}
	int ret;

	ret = mNetwork->getPacket(dispatch);

	dispatch->SetSize(ret);

	return ret;
}

void Client::sendCommand(void)
{
	int outgoingSequence = mOutgoingSequence & (mCommandHistorySize-1);

	Dispatch* dispatch = new Dispatch();

	dispatch->WriteByte(mMessageFrame);						// type
	dispatch->WriteShort(mOutgoingSequence);
	dispatch->WriteShort(mIncomingSequence);

	// Build delta-compressed move command
	buildDeltaMoveCommand(dispatch);

	// Send the packet
	sendPacket(dispatch);

	// Store the command to the input client's history
	memcpy(&mClientCommandToServerArray[outgoingSequence], &mClientCommandToServer, sizeof(Command));
}

void Client::buildDeltaMoveCommand(Dispatch* dispatch)
{
	int flags = 0;
	int last = (mOutgoingSequence - 1) & (mCommandHistorySize-1);

	// Check what needs to be updated
	if(mClientCommandToServerArray[last].mKey != mClientCommandToServer.mKey)
	{
		flags |= mCommandKey;
	}

	if(mClientCommandToServerArray[last].mMilliseconds != mClientCommandToServer.mMilliseconds)
	{
		flags |= mCommandMilliseconds;
	}

	// Add to the message
	
	//Flags
	dispatch->WriteByte(flags);

	// Key
	if(flags & mCommandKey)
	{
		dispatch->WriteByte(mClientCommandToServer.mKey);
	}

	if(flags & mCommandMilliseconds)
	{
		dispatch->WriteByte(mClientCommandToServer.mMilliseconds);
	}
}
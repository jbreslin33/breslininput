#include "client.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../clientside/network/datagramSocket.h"
#include "../../clientside/network/datagramPacket.h"
#include "../../clientside/dispatch/dispatch.h"

//client side client constructor, one on each client machine, i.e. one instance per machine.
Client::Client(const char *localIP, const char *remoteIP, int serverPort)
{
	//commmands
	mCommandHistorySize = 64;

	mSizeOfDispatch = 1400;
	mShape = NULL; //to be filled when we actually create the shape

	mConnectionState	= mMessageDisconnected;

	mOutgoingSequence		= 1;
	mIncomingSequence		= 0;
	mIncomingAcknowledged	= 0;
	mDroppedPackets			= 0;

	// Save server's address information for later use
	mServerIP = remoteIP;
	mServerPort = serverPort;

	LogString("Server's information: IP address: %s, port: %d", remoteIP, mServerPort);

	// Create client socket
	mDatagramSocket = new DatagramSocket();
}

Client::~Client()
{
	mDatagramSocket->close();
	delete mDatagramSocket;
}
/**********  SENDS  ****/
void Client::sendConnect(const char *name)
{
	mConnectionState = mMessageConnecting;

	Dispatch* dispatch = new Dispatch(mSizeOfDispatch);
	dispatch->WriteByte(mMessageConnect);
	dispatch->WriteString(name);

	sendPacket(dispatch);
}

void Client::sendDisconnect(void)
{
	Dispatch* dispatch = new Dispatch(mSizeOfDispatch);

	dispatch->WriteByte(mMessageDisconnect);

	sendPacket(dispatch);
	reset();

	mConnectionState = mMessageDisconnecting;
}

void Client::reset(void)
{
	mConnectionState = mMessageDisconnected;

    mOutgoingSequence                = 1;
    mIncomingSequence                = 0;
    mIncomingAcknowledged			 = 0;
    mDroppedPackets                  = 0;
}

void Client::sendPacket(Dispatch *dispatch)
{
    DatagramPacket* packet = new DatagramPacket(dispatch->mCharArray,dispatch->GetSize(),mServerIP,mServerPort);
	
	mDatagramSocket->send(packet);

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
	if(!mDatagramSocket)
	{
		return 0;
	}
	int ret;

	ret = mDatagramSocket->getPacket(dispatch->mCharArray);
	if(ret <= 0)
		return 0;

	dispatch->SetSize(ret);

	// Parse system messages
	parsePacket(dispatch);

	return ret;
}

void Client::parsePacket(Dispatch *mes)
{
	mes->BeginReading();
	int type = mes->ReadByte();

	// Check if the type is a positive number
	// = is the packet sequenced
	if(type > 0)
	{
		unsigned short sequence		= mes->ReadShort();
		unsigned short sequenceAck	= mes->ReadShort();

		if(sequence <= mIncomingSequence)
		{
			LogString("Client: (sequence: %d <= incoming seq: %d)",
				sequence, mIncomingSequence);

			LogString("Client: Sequence mismatch");
		}

		mDroppedPackets = sequence - mIncomingSequence + 1;

		mIncomingSequence = sequence;
		mIncomingAcknowledged = sequenceAck;
	}

	// Parse trough the system messages
	switch(type)
	{
	case mMessageConnect:
		mConnectionState = mMessageConnected;

		LogString("LIBRARY: Client: got connect confirmation");
		break;

	case mMessageDisconnect:
		mConnectionState = mMessageDisconnected;

		LogString("LIBRARY: Client: got disconnect confirmation");
		break;

	case mMessageAddShape:
		//LogString("LIBRARY: Client: adding a shape");
		break;

	case mMessageRemoveShape:
		LogString("LIBRARY: Client: removing a client");
		break;
	}
}

void Client::sendCommand(void)
{
	if(mConnectionState != mMessageConnected)
		return;

	int outgoingSequence = mOutgoingSequence & (mCommandHistorySize-1);

	Dispatch* dispatch = new Dispatch(1400);

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
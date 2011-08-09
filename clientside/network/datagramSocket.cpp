#include "datagramSocket.h"

#include "../tdreamsock/dreamSockLog.h"
#include "datagramPacket.h"

#ifdef WIN32
#include "../tdreamsock/dreamWinSock.h"
#else
#include "../tdreamsock/dreamLinuxSock.h"
#endif

#ifdef WIN32
//do nothing
#else
#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#endif

#include "../../clientside/client/client.h"

DatagramSocket::DatagramSocket(const char serverIP[32], int serverPort )
{
#ifdef WIN32
	mDreamWinSock = new DreamWinSock();
#else
	mDreamLinuxSock = new DreamLinuxSock();
#endif

	mSocket = open();

	if(mSocket == DREAMSOCK_INVALID_SOCKET)
	{
		LogString("ERROR IN CONSTRUCTOR OF SERVER, INVALID SOCKET");
	}

	//ripped from client, since we only have one client on this side let's do it here.
	memset((char *) &sendToAddress, 0, sizeof(sendToAddress));

	u_long inetAddr               = inet_addr(serverIP);
	sendToAddress.sin_port        = htons((u_short) serverPort);
	sendToAddress.sin_family      = AF_INET;
	sendToAddress.sin_addr.s_addr = inetAddr;
}

DatagramSocket::DatagramSocket()
{
#ifdef WIN32
	mDreamWinSock = new DreamWinSock();
#else
	mDreamLinuxSock = new DreamLinuxSock();
#endif

	mSocket = open();
}

DatagramSocket::~DatagramSocket()
{
}

//open
SOCKET DatagramSocket::open()
{
	SOCKET sock;

	struct sockaddr_in address;

	sock = createSocket(DREAMSOCK_UDP);

	if(sock == DREAMSOCK_INVALID_SOCKET)
		return sock;

	setNonBlocking(1);

	LogString("No net interface given, using any interface available");
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	LogString("No port defined, picking one for you");
	address.sin_port = 0;

	address.sin_family = AF_INET;

	// Bind the address to the socket
	if(bind(sock, (struct sockaddr *) &address, sizeof(address)) == -1)
	{
#ifdef WIN32
		errno = WSAGetLastError();
		size_t t = 256;
		LogString("Error code %d: bind() : %s", errno, strerror_s("error",t,errno));
#else
		LogString("Error code %d: bind() : %s", errno, strerror(errno));
#endif

		return DREAMSOCK_INVALID_SOCKET;
	}

	// Get the port number (if we did not define one, we get the assigned port number here)
	socklen_t len = sizeof(address);
	getsockname(sock, (struct sockaddr *) &address, &len);

	LogString("Opening UDP port = %d", ntohs(address.sin_port));

	return sock;
}


SOCKET DatagramSocket::createSocket(int protocol)
{
	int type;
	int proto;

	// Check which protocol to use
	if(protocol == DREAMSOCK_TCP)
	{
		type = SOCK_STREAM;
		proto = IPPROTO_TCP;
	}
	else
	{
		type = SOCK_DGRAM;
		proto = IPPROTO_UDP;
	}

	// Create the socket
	if((mSocket = socket(AF_INET, type, proto)) == -1)
	{
		LogString("createSocket - socket() failed");

#ifdef WIN32
		errno = WSAGetLastError();
		size_t t = 256;
		LogString("Error: socket() code %d : %s", errno, strerror_s("error",t,errno));
#else
		LogString("Error: socket() : %s", strerror(errno));
#endif

		return DREAMSOCK_INVALID_SOCKET;
	}

	return mSocket;
}

int DatagramSocket::setNonBlocking(u_long setMode)
{
	u_long set = setMode;

	// Set the socket option
#ifdef WIN32
	return ioctlsocket(mSocket, FIONBIO, &set);
#else
	return ioctl(mSocket, FIONBIO, &set);
#endif
}

//close
void DatagramSocket::close()
{
#ifdef WIN32
		closesocket(mSocket);
#else
		close(mSocket);
#endif
}

//send
void DatagramSocket::send(int length, char *data, struct sockaddr addr)
{
	int	ret;

	ret = sendto(mSocket, data, length, 0, &addr, sizeof(addr));

	if(ret == -1)
	{
#ifdef WIN32
		errno = WSAGetLastError();

		// Silently handle wouldblock
		if(errno == WSAEWOULDBLOCK)
			return;
		size_t t = 256;
		LogString("Error code %d: sendto() : %s", errno, strerror_s("error",t,errno));
#else
		// Silently handle wouldblock
		if(errno == EWOULDBLOCK)
			return;

		LogString("Error code %d: sendto() : %s", errno, strerror(errno));
#endif
	}
}

void DatagramSocket::setSendToAddress(const char* serverIP, int serverPort)
{
	//ripped from client, since we only have one client on this side let's do it here.
	memset((char *) &sendToAddress, 0, sizeof(sendToAddress));

	u_long inetAddr               = inet_addr(serverIP);
	sendToAddress.sin_port        = htons((u_short) serverPort);
	sendToAddress.sin_family      = AF_INET;
	sendToAddress.sin_addr.s_addr = inetAddr;
}

void DatagramSocket::send(DatagramPacket* packet)
{
/****
error check...
***/

	// Check that everything is set up
	//if(!mDatagramSocket->mSocket || mConnectionState == mMessageDisconnected)
	if(!mSocket)
	{
		LogString("SendPacket error: Could not send because the client is disconnected");
		return;
	}

	// If the message overflowed do not send it
	//if(dispatch->GetOverFlow())
	//packet->;
	//{
	//	LogString("SendPacket error: Could not send because the buffer overflowed");
	//	return;
	//}
/****
actual send...
***/
	setSendToAddress(packet->mAddress,packet->mPort);

	send(packet->mLength, packet->mDataBuffer,
			*(struct sockaddr *) &sendToAddress);
}


//receive
int DatagramSocket::getPacket(char *data)
{
	int ret;
	struct sockaddr tempFrom;
	socklen_t fromlen;

	fromlen = sizeof(tempFrom);

	ret = recvfrom(mSocket, data, 1400, 0, (struct sockaddr *) &tempFrom, &fromlen);

	if(ret == -1)
	{
#ifdef WIN32
		errno = WSAGetLastError();

		// Silently handle wouldblock
		if(errno == WSAEWOULDBLOCK)
			return ret;

		if(errno == WSAEMSGSIZE)
		{
			// ERROR: Oversize packet

			return ret;
		}
		size_t t = 256;
//if (mClient)
//	if (mClient->mConnectionState == 1)
		LogString("Error code %d: recvfrom() : %s", errno, strerror_s("error",t,errno));
#else
		// Silently handle wouldblock
		if(errno == EWOULDBLOCK || errno == ECONNREFUSED)
			return ret;

		LogString("Error code %d: recvfrom() : %s", errno, strerror(errno));
#endif

		return ret;
	}



	return ret;
}


void DatagramSocket::receive(DatagramPacket* packet)
{
	int ret;
	struct sockaddr tempFrom;
	socklen_t fromlen;

	fromlen = sizeof(tempFrom);

//	ret = recvfrom(mSocket, data, 1400, 0, (struct sockaddr *) &tempFrom, &fromlen);
	ret = recvfrom(mSocket, packet->mDataBuffer, 1400, 0, (struct sockaddr *) &tempFrom, &fromlen);
	if(ret == -1)
	{
#ifdef WIN32
		errno = WSAGetLastError();

		// Silently handle wouldblock
		if(errno == WSAEWOULDBLOCK)
		{
			LogString("receive error:%d",errno);
		}
		if(errno == WSAEMSGSIZE)
		{
			LogString("receive error:%d",errno);
		}
		size_t t = 256;
//if (mClient)
//	if (mClient->mConnectionState == 1)
		LogString("Error code %d: recvfrom() : %s", errno, strerror_s("error",t,errno));
#else
		// Silently handle wouldblock
		if(errno == EWOULDBLOCK || errno == ECONNREFUSED)
			return ret;

		LogString("Error code %d: recvfrom() : %s", errno, strerror(errno));
#endif

			LogString("receive ret:%d",ret);
	}



	//return ret;
}

#include "network.h"

#include "../message/message.h"
#include "../../tdreamsock/dreamSockLog.h"

#ifdef WIN32
#include "../../tdreamsock/dreamWinSock.h"
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

#include "../tdreamsock/dreamLinuxSock.h"
#endif

#include "../../clientside/client/client.h"

Network::Network(const char serverIP[32], int serverPort )
{

#ifdef WIN32
	mDreamWinSock = new DreamWinSock();
#else
	mDreamLinuxSock = new DreamLinuxSock();
#endif

	mSocket = dreamSock_OpenUDPSocket();

	if(mSocket == DREAMSOCK_INVALID_SOCKET)
	{
		//return DREAMSOCK_SERVER_ERROR;
		LogString("ERROR IN CONSTRUCTOR OF SERVER, INVALID SOCKET");
	}

	//ripped from client, since we only have one client on this side let's do it here.
	memset((char *) &sendToAddress, 0, sizeof(sendToAddress));

	u_long inetAddr = inet_addr(serverIP);
	sendToAddress.sin_port = htons((u_short) serverPort);
	sendToAddress.sin_family = AF_INET;
	sendToAddress.sin_addr.s_addr = inetAddr;
}

Network::~Network()
{
}

void Network::dreamSock_Shutdown(void)
{
	LogString("Shutting down dreamSock");

#ifdef WIN32
	WSACleanup();
#endif
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
SOCKET Network::dreamSock_Socket(int protocol)
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
		LogString("dreamSock_Socket - socket() failed");

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

int Network::dreamSock_SetNonBlocking(u_long setMode)
{
	u_long set = setMode;

	// Set the socket option
#ifdef WIN32
	return ioctlsocket(mSocket, FIONBIO, &set);
#else
	return ioctl(mSocket, FIONBIO, &set);
#endif
}

int Network::dreamSock_SetBroadcasting(int mode)
{
	// make it broadcast capable
	if(setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, (char *) &mode, sizeof(int)) == -1)
	{
		LogString("DreamSock_SetBroadcasting failed");

#ifdef WIN32
		int err = WSAGetLastError();
		size_t t = 256;
		LogString("Error code %d: setsockopt() : %s", err, strerror_s("error",t,err));
#else
		LogString("Error code %d: setsockopt() : %s", errno, strerror(errno));
#endif

		return DREAMSOCK_INVALID_SOCKET;
	}

	return 0;
}

int Network::dreamSock_StringToSockaddr(const char *addressString, struct sockaddr *sadr)
{
	char copy[128];

	memset(sadr, 0, sizeof(struct sockaddr));

	struct sockaddr_in *addressPtr = (struct sockaddr_in *) sadr;

	addressPtr->sin_family = AF_INET;
	addressPtr->sin_port = htons(0);
#ifdef WIN32
	size_t t = 256;
	strcpy_s(copy,t, addressString);
#else
	strcpy(copy,addressString);
#endif
	// If the address string begins with a number, assume an IP address
	if(copy[0] >= '0' && copy[0] <= '9')
	{
		*(int *) &addressPtr->sin_addr = inet_addr(copy);
		return 0;
	}
	else return 1;
}

SOCKET Network::dreamSock_OpenUDPSocket()
{
	SOCKET sock;

	struct sockaddr_in address;

	sock = dreamSock_Socket(DREAMSOCK_UDP);

	if(sock == DREAMSOCK_INVALID_SOCKET)
		return sock;

	dreamSock_SetNonBlocking(1);
	dreamSock_SetBroadcasting(1);

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

void Network::dreamSock_CloseSocket()
{
#ifdef WIN32
		closesocket(mSocket);
#else
		close(mSocket);
#endif
}

int Network::dreamSock_GetPacket(char *data)
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

void Network::dreamSock_SendPacket(int length, char *data, struct sockaddr addr)
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

int Network::dreamSock_GetCurrentSystemTime(void)
{
#ifndef WIN32
	return mDreamLinuxSock->dreamSock_Linux_GetCurrentSystemTime();
#else
	return mDreamWinSock->dreamSock_Win_GetCurrentSystemTime();
#endif
}


void Network::sendPacket(Message *theMes)
{
	dreamSock_SendPacket(theMes->GetSize(), theMes->data,
			*(struct sockaddr *) &sendToAddress);

}
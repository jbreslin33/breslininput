#include "network.h"

#include "../message/message.h"
#include "../tdreamsock/dreamSockLog.h"

#ifdef WIN32
#include "../tdreamsock/dreamWinSock.h"
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

#include "../../tdreamsock/dreamLinuxSock.h"
#endif

#include "../client/client.h"


Network::Network()
{

	mClient = NULL;
#ifdef WIN32
	mDreamWinSock = new DreamWinSock();
#else
	mDreamLinuxSock = new DreamLinuxSock();
#endif

	mSocket = 0;
}

Network::Network(Client* client, const char netInterface[32], int port)
{
	mClient = client;
#ifdef WIN32
	mDreamWinSock = new DreamWinSock();
#else
	mDreamLinuxSock = new DreamLinuxSock();
#endif

	mSocket = dreamSock_OpenUDPSocket(netInterface, port);


	if(mSocket == DREAMSOCK_INVALID_SOCKET)
	{
		//return DREAMSOCK_SERVER_ERROR;
		LogString("ERROR IN CONSTRUCTOR OF SERVER, INVALID SOCKET");
	}
}

Network::Network(const char netInterface[32], int port)
{
	mClient = NULL;
#ifdef WIN32
	mDreamWinSock = new DreamWinSock();
#else
	mDreamLinuxSock = new DreamLinuxSock();
#endif

	mSocket = dreamSock_OpenUDPSocket(netInterface, port);


	if(mSocket == DREAMSOCK_INVALID_SOCKET)
	{
		//return DREAMSOCK_SERVER_ERROR;
		LogString("ERROR IN CONSTRUCTOR OF SERVER, INVALID SOCKET");
	}
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
	SOCKET sock;

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
	if((sock = socket(AF_INET, type, proto)) == -1)
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

	return sock;
}

int Network::dreamSock_SetNonBlocking(SOCKET sock, u_long setMode)
{
	u_long set = setMode;

	// Set the socket option
#ifdef WIN32
	return ioctlsocket(sock, FIONBIO, &set);
#else
	return ioctl(sock, FIONBIO, &set);
#endif
}

int Network::dreamSock_SetBroadcasting(SOCKET sock, int mode)
{
	// make it broadcast capable
	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *) &mode, sizeof(int)) == -1)
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

SOCKET Network::dreamSock_OpenUDPSocket(const char *netInterface, int port)
{
	SOCKET sock;

	struct sockaddr_in address;

	sock = dreamSock_Socket(DREAMSOCK_UDP);

	if(sock == DREAMSOCK_INVALID_SOCKET)
		return sock;

	dreamSock_SetNonBlocking(sock, 1);
	dreamSock_SetBroadcasting(sock, 1);

	// If no address string provided, use any interface available
	if(!netInterface || !netInterface[0] || !strcmp(netInterface, "localhost"))
	{
		LogString("No net interface given, using any interface available");
		address.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		LogString("Using net interface = '%s'", netInterface);
		dreamSock_StringToSockaddr(netInterface, (struct sockaddr *) &address);
	}

	// If no port number provided, use any port number available
	if(port == 0)
	{
		LogString("No port defined, picking one for you");
		address.sin_port = 0;
	}
	else
	{
		address.sin_port = htons((short) port);
	}

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

void Network::dreamSock_CloseSocket(SOCKET sock)
{
#ifdef WIN32
		closesocket(sock);
#else
		close(sock);
#endif
}

int Network::dreamSock_GetPacket(SOCKET sock, char *data, struct sockaddr *from)
{
	int ret;
	struct sockaddr tempFrom;
	socklen_t fromlen;

	fromlen = sizeof(tempFrom);

	ret = recvfrom(sock, data, 1400, 0, (struct sockaddr *) &tempFrom, &fromlen);

	// Copy the address to the from pointer
	if(from != NULL)
		*(struct sockaddr *) from = tempFrom;

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
if (mClient)
	if (mClient->mConnectionState == 1)
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

void Network::dreamSock_SendPacket(SOCKET sock, int length, char *data, struct sockaddr addr)
{
	int	ret;

	ret = sendto(sock, data, length, 0, &addr, sizeof(addr));

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

void Network::dreamSock_Broadcast(SOCKET sock, int length, char *data, int port)
{
	struct sockaddr_in servaddr;
	socklen_t len;

	// Use broadcast address
	u_long inetAddr = inet_addr("255.255.255.255");

	// Fill address information structure
	memset(&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family		= AF_INET;
	servaddr.sin_port		= htons(port);	
	servaddr.sin_addr.s_addr = inetAddr;

	len = sizeof(servaddr);

	// Broadcast!
	int ret = sendto(sock, data, length, 0, (struct sockaddr *) &servaddr, len);

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

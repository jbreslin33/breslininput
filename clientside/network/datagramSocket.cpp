#include "datagramSocket.h"
#include "../../tdreamsock/dreamSockLog.h"
#include "../../clientside/network/network.h"
#include "../../clientside/network/datagramPacket.h"
#include "../../clientside/message/message.h"

DatagramSocket::DatagramSocket(int port)
{
	
}
DatagramSocket::DatagramSocket()
{

}

DatagramSocket::~DatagramSocket()
{
}

void DatagramSocket::openSocket()
{
	struct sockaddr_in address;

	int type  = SOCK_DGRAM;
	int proto = IPPROTO_UDP;

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

		//return DREAMSOCK_INVALID_SOCKET;
			LogString("Invalid Socket");
	}

	//sock = dreamSock_Socket(DREAMSOCK_UDP);

	if(mSocket == DREAMSOCK_INVALID_SOCKET)
	{
		LogString("Invalid Socket");
	}
	setNonBlocking(1);

	LogString("No net interface given, using any interface available");
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	LogString("No port defined, picking one for you");
	address.sin_port = 0;

	address.sin_family = AF_INET;

	// Bind the address to the socket
	if(bind(mSocket, (struct sockaddr *) &address, sizeof(address)) == -1)
	{
#ifdef WIN32
		errno = WSAGetLastError();
		size_t t = 256;
		LogString("Error code %d: bind() : %s", errno, strerror_s("error",t,errno));
#else
		LogString("Error code %d: bind() : %s", errno, strerror(errno));
#endif

		//return DREAMSOCK_INVALID_SOCKET;
		LogString("Invalid Socket");
	}

}

void DatagramSocket::send(DatagramPacket* packet)
{
	struct sockaddr_in sendToAddress;
	memset((char *) &sendToAddress, 0, sizeof(sendToAddress));

	//u_long inetAddr               = inet_addr(serverIP);
	u_long inetAddr               = inet_addr(packet->mAddress);
	//sendToAddress.sin_port        = htons((u_short) serverPort);
	sendToAddress.sin_port        = htons((u_short) packet->mPort);
	sendToAddress.sin_family      = AF_INET;
	sendToAddress.sin_addr.s_addr = inetAddr;

	sendPacket(packet->mMessage->GetSize(), packet->mMessage->data, *(struct sockaddr *) &sendToAddress);
}
void DatagramSocket::receive(DatagramPacket* packet)
{
	//Network* network = new Network(packet->mAddress,packet->mPort);
	//network->dreamSock_GetPacket(packet->getData());
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

void DatagramSocket::sendPacket(int length, char *data, struct sockaddr addr)
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
#include "datagramPacket.h"

DatagramPacket::DatagramPacket(Message* message, char* dataBuffer, int length, const char address[32], int port)
{
	mMessage    = message;

	mDataBuffer = dataBuffer;
	mLength     = length;
	mAddress    = address;
	mPort       = port;
}
//void DatagramSocket::dreamSock_SendPacket(int length, char *data, struct sockaddr addr)
DatagramPacket::DatagramPacket(                  char* dataBuffer, int length, const char address[32], int port)
{
	//mMessage    = Null;

	mDataBuffer = dataBuffer;
	mLength     = length;
	mAddress    = address;
	mPort       = port;
}

DatagramPacket::DatagramPacket(Message* message, char* dataBuffer, int length)
{
	mMessage    = message;
	mDataBuffer = dataBuffer;
	mLength     = length;
}
DatagramPacket::~DatagramPacket()
{

}

char* DatagramPacket::getData()
{
	return mDataBuffer;
}

int DatagramPacket::getLength()
{
	return sizeof(mDataBuffer);
}
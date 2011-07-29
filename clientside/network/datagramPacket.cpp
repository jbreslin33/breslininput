#include "datagramPacket.h"

DatagramPacket::DatagramPacket(                  char* dataBuffer, int length, const char address[32], int port)
{
	mDataBuffer = dataBuffer;
	mLength     = length;
	mAddress    = address;
	mPort       = port;
}

DatagramPacket::DatagramPacket(char* dataBuffer, int length)
{
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
	return mLength;
}
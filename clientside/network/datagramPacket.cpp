#include "datagramPacket.h"

DatagramPacket::DatagramPacket(char buf[], int length, const char address[32], int port)
{
	mDataBuffer = buf;
	mLength     = length;
	mAddress    = address;
	mPort       = port;
}
DatagramPacket::DatagramPacket(char buf[], int length)
{
	mDataBuffer = buf;
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
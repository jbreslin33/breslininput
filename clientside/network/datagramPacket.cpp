#include "datagramPacket.h"

DatagramPacket::DatagramPacket(char buffer[32], int length, const char address[32], int port)
{
	mDataBuffer = buffer;
	mLength     = length;
	mAddress    = address;
	mPort       = port;
	

}
DatagramPacket::DatagramPacket(char buffer[32], int length)
{
	mDataBuffer = buffer;
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
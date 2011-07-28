#include "datagramPacket.h"

DatagramPacket::DatagramPacket(Message* message, char buffer[32], int length, const char address[32], int port)
{
	mMessage    = message;
	mDataBuffer = buffer;
	mLength     = length;
	mAddress    = address;
	mPort       = port;
}

DatagramPacket::DatagramPacket(Message* message, char buffer[32], int length)
{
	mMessage    = message;
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
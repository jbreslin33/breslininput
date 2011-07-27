#ifndef DATAGRAMPACKET_H
#define DATAGRAMPACKET_H

class DatagramPacket 
{
public:

char* mDataBuffer;	
	
DatagramPacket(char buffer[32], int length, const char serverIP[32], int serverPort);
DatagramPacket(char buffer[32], int length);
~DatagramPacket();

const char*  mAddress;
int          mPort;

int   mLength;

char* getData();
int   getLength();

};
#endif

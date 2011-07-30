#ifndef DATAGRAMPACKET_H
#define DATAGRAMPACKET_H

class Message;

class DatagramPacket 
{
public:

char* mDataBuffer;	
	
DatagramPacket(char buf[], int length, const char serverIP[32], int serverPort);
DatagramPacket(char buf[], int length);

~DatagramPacket();

const char*  mAddress;
int          mPort;

int   mLength;

char* getData();
int   getLength();

};
#endif

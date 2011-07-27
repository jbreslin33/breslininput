#ifndef DATAGRAMPACKET_H
#define DATAGRAMPACKET_H

class DatagramPacket 
{
public:
DatagramPacket(char buffer[32], int length, const char serverIP[32], int serverPort);
DatagramPacket(char buffer[32], int length);
~DatagramPacket();

};
#endif

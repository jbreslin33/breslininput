#ifndef DATAGRAMSOCKET_H
#define DATAGRAMSOCKET_H

class DatagramPacket;

class DatagramSocket 
{
public:
DatagramSocket(int serverPort);
~DatagramSocket();

void send(DatagramPacket* packet);
void receive(DatagramPacket* packet);
};
#endif

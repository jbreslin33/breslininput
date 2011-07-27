#ifndef DATAGRAMSOCKET_H
#define DATAGRAMSOCKET_H

class DatagramPacket;
class Network;

class DatagramSocket 
{
public:
DatagramSocket(int serverPort);
DatagramSocket();
~DatagramSocket();

void send(DatagramPacket* packet);
void receive(DatagramPacket* packet);
};
#endif

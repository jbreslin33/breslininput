#include "datagramSocket.h"
#include "../../tdreamsock/dreamSockLog.h"
#include "../../clientside/network/network.h"
#include "../../clientside/network/datagramPacket.h"

DatagramSocket::DatagramSocket(int port)
{
	
}
DatagramSocket::DatagramSocket()
{

}

DatagramSocket::~DatagramSocket()
{
}

void DatagramSocket::send(DatagramPacket* packet)
{
	
}
void DatagramSocket::receive(DatagramPacket* packet)
{
	Network* network = new Network(packet->mAddress,packet->mPort);
	network->dreamSock_GetPacket(packet->getData());
}
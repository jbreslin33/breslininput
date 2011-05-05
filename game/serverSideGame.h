#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <string.h>
#include <vector>
#include "../tdreamsock/dreamSock.h"

class ServerSidePlayer;
class ServerSideClient;
class Command;

#define COMMAND_HISTORY_SIZE		64

#define KEY_UP						1
#define KEY_DOWN					2
#define KEY_LEFT					4
#define KEY_RIGHT					8

#define CMD_KEY						1
#define CMD_ORIGIN					4

#define USER_MES_FRAME				1
#define USER_MES_NONDELTAFRAME		2
#define USER_MES_SERVEREXIT			3

class ServerSideGame
{
private:
	dreamServer	*mNetworkServer;

	std::vector<ServerSideClient*> mClientVector;

	int		mRealTime;				// Real server up-time in ms
	int		mServerTime;				// Server frame * 100 ms
	long	mFramenum;

public:
	ServerSideGame();
	~ServerSideGame();

	// Network.cpp
	void	ReadPackets(void);
	void	SendCommand(void);
	void	SendExitNotification(void);
	void	ReadDeltaMoveCommand(dreamMessage *mes, ServerSideClient *client);
	void	BuildMoveCommand(dreamMessage *mes, ServerSideClient *client);
	void	BuildDeltaMoveCommand(dreamMessage *mes, ServerSideClient *client);

	// Server.cpp
	int		InitNetwork();
	void	ShutdownNetwork(void);
	void	AddClient(void);
	void	RemoveClient(ServerSideClient* client);
	void	RemoveClients(void);
	void	Frame(int msec);

};

#endif

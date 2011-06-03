#ifndef GAME_H
#define GAME_H

#include <string.h>

#include <vector>

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

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

//class Network;
class Message;
class Command;
class Server;
class Client;
class Shape;

class Game
{
public:

	Ogre::Root* mRoot;
	
	Server	*mServer;

	int		mRealTime;				// Real server up-time in ms
	int		mServerTime;				// Server frame * 100 ms

	long	mFramenum;

	std::vector<Shape*> mShapeVector;	//every tangible item in game world..


public:
	Game();
	~Game();

	//player
	void createShape(Client* client, int runningIndex);

	// Network.cpp

	void	SendCommand(void);
	void	SendExitNotification(void);
	void	ReadDeltaMoveCommand(Message *mes, Client *client);
	void	BuildMoveCommand(Message *mes, Shape* shape);
	void	BuildDeltaMoveCommand(Message *mes, Shape* shape);

	// Server.cpp

	void	ShutdownNetwork(void);
	void	Frame(int msec);

};

#endif

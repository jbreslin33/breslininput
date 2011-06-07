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
	
	Server	*mServer;  //go between for game(contains game logic) and Network(handles sending message across internets)

	int		mRealTime;				// Real server up-time in ms
	int		mServerTime;				// Server frame * 100 ms
	long	mFramenum;

	int mRunningShapeIndex;

	std::vector<Shape*> mShapeVector;	//every tangible item in game world..


public:
	Game();
	~Game();

	//shapes
	void createShape(Client* client);
	void createAIShape();
	void RemoveShape (Shape* shape);
	
	// Network
	void	SendCommand(void);
	void	SendExitNotification(void);
	void	ReadDeltaMoveCommand(Message *mes, Client *client);
	void    fakeReadDeltaMoveCommand(Shape* shape);
	void	BuildMoveCommand(Message *mes, Shape* shape);
	void	BuildDeltaMoveCommand(Message *mes, Shape* shape);

	// Server
	void	ShutdownNetwork(void);

	//time
	void	Frame(int msec);

};

#endif

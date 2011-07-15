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

//flag
#define CMD_KEY						1
#define CMD_MILLISECONDS            2
#define CMD_ORIGIN_X                4
#define CMD_ORIGIN_Y                8
#define CMD_ORIGIN_Z               16 
#define CMD_ROTATION_X             32
#define CMD_ROTATION_Z             64
#define CMD_SKIPNEXTSHAPE         128
//skip next guy???

#define USER_MES_FRAME				1
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

	//time
	int		mRealTime;				// Real server up-time in ms
	int		mServerTime;				// Server frame * 100 ms
	long	mFramenum;
	int     mFrameTime;

	//shapes
	int mRunningShapeIndex;
	int mSpreadOutAIIndex;
	std::vector<Shape*> mShapeVector;	//every tangible item in game world..

public:
	Game();
	~Game();

	//shapes
	void         createClientAvatar(Client* client);
	void         createAIShape();
	void         removeShape (Shape* shape);
	unsigned int getOpenIndex();
	
	// Network
	void	sendCommand(void);
	void	sendExitNotification(void);
	void	readDeltaMoveCommand(Message *mes, Client *client);
	void	buildDeltaMoveCommand(Message *mes, Shape* shape);

	//time
	void	frame(int msec);
	
	//collision detection
	void	checkCollisions(void);

	//scope
 	bool    checkScope(Client* client, Shape* shape);

};

#endif

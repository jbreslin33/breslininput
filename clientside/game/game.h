#ifndef GAME_H
#define GAME_H

#include "../../game/BaseApplication.h"

#include "../../clientside/shape/shape.h"

#define VK_ESCAPE 0x1B
#define VK_UP 0x26
#define VK_DOWN 0x28
#define VK_LEFT 0x25
#define VK_RIGHT 0x27
#define VK_SPACE 0x20

extern bool keys[256];

#define COMMAND_HISTORY_SIZE	64

#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16

#define CMD_KEY					1
#define CMD_ORIGIN				4

#define USER_MES_FRAME			1
#define USER_MES_NONDELTAFRAME	2
#define USER_MES_SERVEREXIT		3

#define USER_MES_KEEPALIVE		12

#ifdef WIN32
class DreamWinSock;
#endif
class Network;
class Message;
class Client;
class MovableTextOverlay;


class Game : public BaseApplication
{
public:
	
	Game(const char* serverIP);
	~Game();

	//Players
	void	MoveServerPlayer(void);

	//Game
	void	AddShape   (int local, int index, char *name);
	void    RemoveShape(int index);
	void	Shutdown    (void);
	void	CheckKeys   (void);
	void	Frame       (void);
	void	RunNetwork  (int msec);

	// Network
	void	ReadPackets             (void);
	void	SendCommand             (void);
	void	SendRequestNonDeltaFrame(void);
	void	ReadMoveCommand         (Message *mes, Shape *player);
	void	ReadDeltaMoveCommand    (Message *mes, Shape *player);
	void	BuildDeltaMoveCommand   (Message *mes, Shape *player);
	void	Disconnect              (void);
	void	SendStartGame           (void);

	//Ogre
	bool         processUnbufferedInput(const Ogre::FrameEvent& evt);
    virtual void createScene           (void);
    virtual bool frameRenderingQueued  (const Ogre::FrameEvent& evt);

	// Network variables
	Client* mClient;
	const char*  mServerIP;

	//client Variables
	Shape* mLocalShape;		// Pointer to the local shape 
	Shape* mInputShape;			// Handles all keyboard input
	std::vector<Shape*> mShapeVector;	
	vector<MovableTextOverlay*> myVect;
	
	//time
	float mFrameTime;
	float mRenderTime;
	int   mOldTime;

	//initialize
	bool mInit;
	bool mNetworkShutdown;
#ifdef WIN32
	DreamWinSock* mDreamWinSock;
#endif
};

#endif

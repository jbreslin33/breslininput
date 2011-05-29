#ifndef CLIENTSIDEGAME_H
#define CLIENTSIDEGAME_H

#include "BaseApplication.h"

#include "../shape/clientSideShape.h"

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

class ClientSideGame : public BaseApplication
{
public:
	
	ClientSideGame(const char* serverIP);
	~ClientSideGame();

	//Players
	void	MoveServerPlayer(void);

	//Game
	void	AddPlayer   (int local, int index, char *name);
	void	Shutdown    (void);
	void	CheckKeys   (void);
	void	Frame       (void);
	void	RunNetwork  (int msec);

	// Network
	void	ReadPackets             (void);
	void	SendCommand             (void);
	void	SendRequestNonDeltaFrame(void);
	void	ReadMoveCommand         (Message *mes, ClientSideShape *player);
	void	ReadDeltaMoveCommand    (Message *mes, ClientSideShape *player);
	void	BuildDeltaMoveCommand   (Message *mes, ClientSideShape *player);
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
	ClientSideShape* mLocalShape;		// Pointer to the local client in the client list
	ClientSideShape*  mInputShape;			// Handles all keyboard input
	std::vector<ClientSideShape*> mShapeVector;	

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

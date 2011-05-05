#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include "BaseApplication.h"
#include "../tdreamsock/dreamSock.h"

#include "../client/clientSideClient.h"

#define VK_ESCAPE 0x1B
#define VK_UP 0x26
#define VK_DOWN 0x28
#define VK_LEFT 0x25
#define VK_RIGHT 0x27

extern bool keys[256];

#define COMMAND_HISTORY_SIZE	64

#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8

#define CMD_KEY					1
#define CMD_ORIGIN				4

#define USER_MES_FRAME			1
#define USER_MES_NONDELTAFRAME	2
#define USER_MES_SERVEREXIT		3

#define USER_MES_KEEPALIVE		12


class ClientSideGame : public BaseApplication
{
public:
	ClientSideGame(const char* serverIP);
	~ClientSideGame();

	//Players
	void	MoveServerPlayer(void);

	//Game
	void	AddClient(int local, int index, char *name);
	void	RemoveClient(int index);
	void	Shutdown(void);
	void	CheckKeys(void);
	void	Frame(void);
	void	RunNetwork(int msec);

	// Network
	void	ReadPackets(void);
	void	SendCommand(void);
	void	SendRequestNonDeltaFrame(void);
	void	ReadMoveCommand(dreamMessage *mes, ClientSideClient *client);
	void	ReadDeltaMoveCommand(dreamMessage *mes, ClientSideClient *client);
	void	BuildDeltaMoveCommand(dreamMessage *mes, ClientSideClient *theClient);
	void	StartConnection();
	void	Connect(void);
	void	Disconnect(void);
	void	SendStartGame(void);

	//Ogre
	bool processUnbufferedInput(const Ogre::FrameEvent& evt);
    void createPlayer(ClientSideClient* client, int index);
	void createServerPlayer(ClientSideClient* client, int index);
    virtual void createScene(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// Network variables
	dreamClient* mNetworkClient;
	const char*  mServerIP;

	//client Variables
	ClientSideClient* mLocalClient;		// Pointer to the local client in the client list
	ClientSideClient  mInputClient;			// Handles all keyboard input
	std::vector<ClientSideClient*> mClientVector;	

	//time
	float mFrametime;
	float mRendertime;
	int   mOldTime;

	//initialize
	bool mInit;
};

#endif

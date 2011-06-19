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

//flag1
#define CMD_ORIGIN_X                1
#define CMD_ORIGIN_Z                2
#define CMD_ORIGIN_Y                4
#define CMD_VELOCITY_X              8
#define CMD_VELOCITY_Z             16
#define CMD_VELOCITY_Y             32
#define CMD_ROTATION_X             64
#define CMD_ROTATION_Z            128

//flag2
#define CMD_KEY						1
#define CMD_MILLISECONDS            2

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

	//Server player 
	//void	MoveServerPlayer(void); //for debugging and still some actual functionality
	void moveGhostShapes();
	
	//Game
	void AddShape(int local, int ind, char *name, float originX, float originZ, float originY,
					float velocityX, float velocityZ, float velocityY, float rotationX, float rotationZ);
	OgreShape* AddGhostShape(int ind,Vector3D* position, Vector3D* velocity, Vector3D* rotation);
	void    RemoveShape(int index);
	void	Shutdown    (void);
	void	CheckKeys   (void);
	void	Frame       (void);
	void	RunNetwork  (int msec);

	// Network
	void	ReadPackets             (void);
	void	SendCommand             (void);
	void	ReadDeltaMoveCommand    (Message *mes, Shape *player);
	void	BuildDeltaMoveCommand   (Message *mes);
	void	Disconnect              (void);
	void	SendStartGame           (void);

	//Ogre
	void         processUnbufferedInput();
    virtual void createScene           (void);
    virtual bool frameRenderingQueued  (const Ogre::FrameEvent& evt);

	// Network variables
	Client* mClient;
	const char*  mServerIP;

	//Shapes
	std::vector<Shape*> mShapeVector;	 //all shapes in the client world
	std::vector<Shape*> mShapeGhostVector;	 //all shapes in the client world's ghost 

	vector<MovableTextOverlay*> myVect;  //for writing above shapes head
	
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

	void gameLoop();
};

#endif

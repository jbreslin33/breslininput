#ifndef OGREGAME_H
#define OGREGAME_H

#include "BaseApplication.h"

#include "../../clientside/shape/ogreDynamicShape.h"

#ifdef WIN32

class DreamWinSock;
#else
class DreamLinuxSock;
#endif

#define VK_ESCAPE 0x1B
#define VK_UP 0x26
#define VK_DOWN 0x28
#define VK_LEFT 0x25
#define VK_RIGHT 0x27
#define VK_SPACE 0x20

#define COMMAND_HISTORY_SIZE	64

#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_SPACE				16


//skip next guy???

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
class Dispatch;

class OgreGame : public BaseApplication
{
public:
	
	OgreGame(const char* serverIP);
	~OgreGame();

	//Server player 
	//void	MoveServerPlayer(void); //for debugging and still some actual functionality
	void moveGhostShapes(DynamicShape* shape);
	
	//Game
	DynamicShape* AddShape(Game* game, int local, int ind, char *name, float originX, float originZ, float originY,
					float velocityX, float velocityZ, float velocityY, float rotationX, float rotationZ);
	DynamicShape* AddGhostShape(Game* game, int ind,Vector3D* position, Vector3D* velocity, Vector3D* rotation);
	DynamicShape* getDynamicShape(int id);
	void    RemoveShape(int index);
	void	Shutdown    (void);
	void	CheckKeys   (void);
	void	Frame       (void);
	void	RunNetwork  (int msec);
	void    checkForShutdown();
	bool    runGraphics();
	void    interpolateFrame();


	// Network
	void	ReadPackets             (void);
	void	SendCommand             (void);
	void	ReadDeltaMoveCommand    (Dispatch* mes);
	void	BuildDeltaMoveCommand   (Dispatch* dispatch);
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
	std::vector<DynamicShape*> mShapeVector;	 //all shapes in the client world
	std::vector<DynamicShape*> mShapeGhostVector;	 //all shapes in the client world's ghost 

	std::vector<MovableTextOverlay*> myVect;  //for writing above shapes head
	
	//time
	float mFrameTime;
	float mRenderTime;
	int   mOldTime;

	//initialize
	bool mInit;
	bool mDatagramSocketShutdown;
	bool mJoinGame;
	bool mPlayingGame;
	bool mInitializeGui;

	//gui
	OgreBites::Button* mJoinButton;

	void buttonHit(OgreBites::Button *button);
	bool mouseMoved( const OIS::MouseEvent &arg );

	
	void hideGui();
	void loadJoinScreen();
	void hideJoinScreen();
	void unloadOtherScreens();
	void initializeGui();

	Ogre::SceneManager* getSceneManager() { return mSceneMgr; }

#ifdef WIN32
DreamWinSock* mDreamWinSock;
#else
DreamLinuxSock* mDreamLinuxSock;
#endif

	void gameLoop();

	//time
#ifndef WIN32
int dreamSock_Linux_GetCurrentSystemTime(void);
#endif

int dreamSock_GetCurrentSystemTime(void);

};

#endif

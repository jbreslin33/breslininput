#ifndef GAME_H
#define GAME_H

#include "BaseApplication.h"

#include "../../clientside/shape/ogreDynamicShape.h"

#ifdef WIN32

class DreamWinSock;
#else
class DreamLinuxSock;
#endif

#ifdef WIN32
class DreamWinSock;
#endif
class Network;
class Message;
class Client;
class MovableTextOverlay;
class Dispatch;

class Game : public BaseApplication
{
public:
	
	Game(const char* serverIP);
	~Game();

	//Server player 
	void moveGhostShapes(DynamicShape* shape);
	
	//Game
	DynamicShape* AddShape(Game* game, int local, int ind, char *name, float originX, float originZ, float originY,
					float velocityX, float velocityZ, float velocityY, float rotationX, float rotationZ);
	DynamicShape* AddGhostShape(Game* game, int ind,Vector3D* position, Vector3D* velocity, Vector3D* rotation);
	DynamicShape* getDynamicShape(int id);
	void    RemoveShape(int index);
	void	Shutdown    (void);
	void	RunNetwork  (int msec);
	void    checkForShutdown();
	bool    runGraphics();
	void    interpolateFrame();


	// Network
	void	ReadPackets             (void);
	void	SendCommand             (void);
	void	ReadDeltaMoveCommand    (Dispatch* mes);
	void	BuildDeltaMoveCommand   (Dispatch* dispatch);
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

	//keys
	bool keys[256];
	int mKeyUp;
	int mKeyDown;
	int mKeyLeft;
	int mKeyRight;
	int mKeySpace;

	//commands
	int mCommandHistorySize;

	//messages
	static const char mMessageFrame = 1;
	static const char mMessageNonDeltaFrame = 2;
	static const char mMessageServerExit = 3;
	static const char mMessageKeepAlive = 12;
	
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

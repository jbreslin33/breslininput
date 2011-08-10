#ifndef GAME_H
#define GAME_H

#include "BaseApplication.h"

//command
#include "../../command/command.h"

class Client;
class MovableTextOverlay;
class Dispatch;
class DynamicShape;
class Time;
class Network;

class Game : public BaseApplication
{
public:
	
	Game(const char* serverIP);
	~Game();

	/***************************************
			    MEMBER VARIABLES
	***************************************/

	//Shapes
	std::vector<DynamicShape*> mShapeVector;	 //all shapes in the client world
	std::vector<DynamicShape*> mShapeGhostVector;	 //all shapes in the client world's ghost 

	std::vector<MovableTextOverlay*> myVect;  //for writing above shapes head

	//commands
	int mCommandHistorySize;
	Command mClientCommandToServerArray[64];
	Command	mClientCommandToServer; //for the human moves to be sent off to server

	// Dispatches
	static const char mCommandKey          = 1;
	static const char mCommandMilliseconds = 2;

	static const char mMessageFrame = 1;
	static const char mMessageConnect     = -101;
	static const char mMessageDisconnect  = -102;
	static const char mMessageAddShape    = -103;
	static const char mMessageRemoveShape = -104;

	static const char mMessageConnecting    = 0;
	static const char mMessageConnected     = 1;
	static const char mMessageDisconnecting = 2;
	static const char mMessageDisconnected  = 4;

	static const char mMessageNonDeltaFrame = 2;
	static const char mMessageServerExit = 3;
	static const char mMessageKeepAlive = 12;

	//Network
	Network*     mNetwork;
	int			 mServerPort;				// Port
	const char*  mServerIP;

	//commands
	void	sendCommand             ();
	void	buildDeltaMoveCommand   (Dispatch* dispatch);

	//time
	Time* mTime;
	float mFrameTime;
	float mRenderTime;
	int   mOldTime;
	
	//initialize
	bool mInit;
	bool mNetworkShutdown;
	bool mJoinGame;
	bool mPlayingGame;
	bool mInitializeGui;

	//gui
	OgreBites::Button* mJoinButton;

	//keys
	int mKeyUp;
	int mKeyDown;
	int mKeyLeft;
	int mKeyRight;
	int mKeySpace;

	/***************************************
			          METHODS
	***************************************/
	//Admin
	void	shutdown    ();
	void	gameLoop();

	//shape
	void addShape    (bool b, Dispatch* dispatch);
	void removeShape (Dispatch* dispatch);

	DynamicShape* getDynamicShape      (  int id);

	void		  frame                (Dispatch* dispatch);
	void          interpolateFrame     ();

	// Network
	void	runNetwork  (int msec);
	void	readPackets             ();
	void			sendPacket (Dispatch* dispatch);
	void			sendConnect(const char *name);
	void			sendDisconnect(void);

	//input
	void processUnbufferedInput();
	void buttonHit             (OgreBites::Button *button);
	bool mouseMoved            ( const OIS::MouseEvent &arg );

	//graphics
	Ogre::SceneManager* getSceneManager      () { return mSceneMgr; }
    virtual void        createScene          ();
    virtual bool        frameRenderingQueued (const Ogre::FrameEvent& evt);
	bool                runGraphics          ();

	//gui	
	void hideGui();
	void loadJoinScreen();
	void hideJoinScreen();
	void unloadOtherScreens();
	void initializeGui();

	

};

#endif

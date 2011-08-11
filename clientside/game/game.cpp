#include "game.h"
#include "../tdreamsock/dreamSockLog.h"
#include "../shape/ogreDynamicShape.h"
#include "../client/client.h"
#include "../../math/vector3D.h"
#include "../dispatch/dispatch.h"
#include "../time/time.h"
#include "../network/network.h"

Game::Game(const char* serverIP)
{
	StartLog();

	mServerIP = serverIP;

	//command
	mLastCommandToServer = new Command();
	mCommandToServer = new Command(); 

	// Save server's address information for later use
	mServerIP = serverIP;
	mServerPort = 30004;

	LogString("Server's information: IP address: %s, port: %d", mServerIP, mServerPort);

	// Create client socket
	mNetwork = new Network(mServerIP,mServerPort);

	mTime = new Time();
	mFrameTime		 = 0.0f;
 	mRenderTime		 = 0.0f;
	mOldTime         = 0;
 	mInit			 = false;
	mNetworkShutdown = false;

	mInit = true;

	mJoinGame    = false;
	mPlayingGame = false;
	mInitializeGui = false;

	//keys
	mKeyUp = 1;
	mKeyDown = 2;
	mKeyLeft = 4;
	mKeyRight = 8;
	mKeySpace = 16;

}

Game::~Game()
{
//	delete mClient;
		mNetwork->close();
	delete mNetwork;
}

/*********************************
		START/LOOP/END
**********************************/
void Game::gameLoop()
{
	while(true)
    {
		processUnbufferedInput();
		runNetwork(mRenderTime * 1000);
		interpolateFrame();
		if (!runGraphics())
		{
			break;
		}
		initializeGui();
	}
}

void Game::shutdown(void)
{
	if(!mInit)
		return;
	mInit = false;
	sendDisconnect();
}

/*********************************
		SHAPE
**********************************/
void Game::addShape(bool b, Dispatch* dispatch)
{
	DynamicShape* shape = new OgreDynamicShape(this,dispatch,false);  //you should just need to call this...
}

void Game::removeShape(Dispatch* dispatch)
{
	int index = dispatch->ReadByte();
	
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		if (mShapeVector.at(i)->mIndex == index)
		{
			delete mShapeVector.at(i);
			mShapeVector.erase (mShapeVector.begin()+i);
		}
	}
}

void Game::frame(Dispatch* dispatch)
{
	int newTime;
	int time;

	// Skip sequences
	dispatch->ReadShort();

	newTime = mTime->dreamSock_GetCurrentSystemTime();
	time = newTime - mOldTime;
    mOldTime = newTime;

	while (dispatch->getReadCount() <= dispatch->GetSize())
	{
		if (mNetworkShutdown)
		{
			return;
		}

		mDetailsPanel->setParamValue(11, Ogre::StringConverter::toString(dispatch->GetSize()));

		int id = dispatch->ReadByte();

		DynamicShape* shape = NULL;
		shape = getDynamicShape(id);

		if (shape)
		{
			shape->readDeltaMoveCommand(dispatch);
		}
	}
}

DynamicShape* Game::getDynamicShape(int id)
{
	DynamicShape* shape = NULL;

	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		DynamicShape* curShape = mShapeVector.at(i);
		if (curShape->mIndex == id)
		{
			shape = curShape;
		}
	}

	if(!shape)
	{
		return NULL;
	}
	else
	{
		return shape;
	}
}

void Game::interpolateFrame()
{
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		mShapeVector.at(i)->interpolateTick(mRenderTime);
	}
}

/*********************************
		NETWORK
**********************************/
void Game::runNetwork(int msec)
{
	static int time = 0;
	time += msec;

	readPackets();
	
	// Framerate is too high
	if(time > (1000 / 60))
	{
		sendCommand();
		mFrameTime = time / 1000.0f;
		time = 0;
	}
}

void Game::readPackets()
{
	int type;
	int ret;

	Dispatch* dispatch = new Dispatch();

	while(ret = mNetwork->checkForDispatch(dispatch))
	{
		dispatch->BeginReading();

		type = dispatch->ReadByte();

		switch(type)
		{
			case mMessageAddShape:
				addShape(true,dispatch);
			break;

			case mMessageRemoveShape:
				removeShape(dispatch);
			break;

			case mMessageFrame:
				frame(dispatch);
			break;

			case mMessageServerExit:
				shutdown();
			break;
		}
	}
}
void Game::sendConnect(const char *name)
{
	Dispatch* dispatch = new Dispatch();
	dispatch->WriteByte(mMessageConnect);
	dispatch->WriteString(name);
	mNetwork->send(dispatch);
}

void Game::sendDisconnect()
{
	Dispatch* dispatch = new Dispatch();
	dispatch->WriteByte(mMessageDisconnect);
	mNetwork->send(dispatch);
	mNetwork->reset();
}

void Game::sendCommand(void)
{
	Dispatch* dispatch = new Dispatch();
	dispatch->WriteByte(mMessageFrame);					
	dispatch->WriteShort(mNetwork->mOutgoingSequence);

	// Build delta-compressed move command
	int flags = 0;

	// Check what needs to be updated
	if(mLastCommandToServer->mKey != mCommandToServer->mKey)
	{
		flags |= mCommandKey;
	}

	if(mLastCommandToServer->mMilliseconds != mCommandToServer->mMilliseconds)
	{
		flags |= mCommandMilliseconds;
	}

	// Add to the message
	dispatch->WriteByte(flags);

	if(flags & mCommandKey)
	{
		dispatch->WriteByte(mCommandToServer->mKey);
	}

	if(flags & mCommandMilliseconds)
	{
		dispatch->WriteByte(mCommandToServer->mMilliseconds);
	}
	
	//set 'last' commands for diff
	mLastCommandToServer->mKey = mCommandToServer->mKey;
	mLastCommandToServer->mMilliseconds = mCommandToServer->mMilliseconds;

	// Send the packet
	mNetwork->send(dispatch);
}

/*********************************
		GRAPHICS
**********************************/
void Game::createScene(void)
{
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.75, 0.75, 0.75));

    Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(Ogre::Vector3(250, 150, 250));
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
}

bool Game::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	mRenderTime = evt.timeSinceLastFrame;

    bool ret = BaseApplication::frameRenderingQueued(evt);
	
	return ret;
}

bool Game::runGraphics()
{
	//Pump messages in all registered RenderWindow windows
	WindowEventUtilities::messagePump();

	if (!mRoot->renderOneFrame())
	{
		return false;
	}
	else
	{
		return true;
	}
}

/*********************************
		GUI
**********************************/
void Game::initializeGui()
{
	if (mInitializeGui == true)
	{
		return;
	}
	else
	{
		loadJoinScreen();
		mInitializeGui = true;
	}
}

void Game::loadJoinScreen()
{
	unloadOtherScreens();
	mJoinButton = mTrayMgr->createButton(OgreBites::TL_CENTER, "mJoinButton", "Join Game");
	mTrayMgr->moveWidgetToTray(mJoinButton,OgreBites::TL_CENTER);
	mTrayMgr->showCursor();
}

void Game::hideGui()
{
	hideJoinScreen();
	mTrayMgr->hideCursor();
}

void Game::hideJoinScreen()
{
	mTrayMgr->removeWidgetFromTray(mJoinButton);
    mJoinButton->hide();
}

void Game::unloadOtherScreens()
{
}

/*********************************
		INPUT
**********************************/
void Game::processUnbufferedInput()
{
	mCommandToServer->mKey = 0;
    
	if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) // ESCAPE
    {
		mNetworkShutdown = true;
    }
	else
	{
		mNetworkShutdown = false;
	}
    	
	if (mKeyboard->isKeyDown(OIS::KC_I)) // Forward
    {
		mCommandToServer->mKey |= mKeyUp;
    }

    if (mKeyboard->isKeyDown(OIS::KC_K)) // Backward
    {
		mCommandToServer->mKey |= mKeyDown;
    }

	if (mKeyboard->isKeyDown(OIS::KC_J)) // Left - yaw or strafe
    {
		mCommandToServer->mKey |= mKeyLeft;
    }

    if (mKeyboard->isKeyDown(OIS::KC_L)) // Right - yaw or strafe
    {
		mCommandToServer->mKey |= mKeyRight;
    }

	mCommandToServer->mMilliseconds = (int) (mFrameTime * 1000);
}

void Game::buttonHit(OgreBites::Button *button)
{
	if (button == mJoinButton)
	{
		mJoinGame = true;
		if (mJoinGame && !mPlayingGame)
		{
			sendConnect("myname");
			//LogString("sent connect to server");
			mPlayingGame = true;
		}

		hideGui();
	}
}

bool Game::mouseMoved( const OIS::MouseEvent &arg )
{
    if (mTrayMgr->injectMouseMove(arg)) return true;
	if (mPlayingGame)
	{
		mCameraMan->injectMouseMove(arg);
	}
    return true;
}


/******************** MAIN ***********************/
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		StartLogConsole();
        Game* game = new Game(strCmdLine);
#else
        Game* game = new Game(argv[1]);
#endif
        try {
            game->go();
			game->gameLoop();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
        return 0;
    }

#ifdef __cplusplus
}
#endif

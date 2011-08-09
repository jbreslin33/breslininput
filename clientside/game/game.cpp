#include "game.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../clientside/network/network.h"

#include "../../clientside/shape/ogreDynamicShape.h"

#include "../../clientside/client/client.h"

#include "../../math/vector3D.h"

#include "../dispatch/dispatch.h"

#ifdef WIN32
#include "../../tdreamsock/dreamWinSock.h"
#else
#include "../tdreamsock/dreamLinuxSock.h"
#endif


Game* game;

Game::Game(const char* serverIP)
{
	StartLog();

	mServerIP = serverIP;
 
#ifdef WIN32
	mDreamWinSock = new DreamWinSock();
#else
	mDreamLinuxSock = new DreamLinuxSock();
#endif

	mClient	= new Client("", mServerIP, 30004);

	mFrameTime		 = 0.0f;
 	mRenderTime		 = 0.0f;
	mOldTime         = 0;
 	mInit			 = false;
	mDatagramSocketShutdown = false;

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
	delete mClient;
}

void Game::addShape(Dispatch* dispatch)
{
	int index;
	int local;
	Vector3D* origin   = new Vector3D();
	Vector3D* velocity = new Vector3D(); 
	Vector3D* rotation = new Vector3D();

	char name[50];

	//read dispatch
	local	=    dispatch->ReadByte();
	index		=    dispatch->ReadByte();
	strcpy(name, dispatch->ReadString());
	origin->x =   dispatch->ReadFloat();
	origin->y =   dispatch->ReadFloat();
	origin->z =   dispatch->ReadFloat();
	velocity->x = dispatch->ReadFloat();
	velocity->y = dispatch->ReadFloat();
	velocity->z = dispatch->ReadFloat();
	rotation->x = dispatch->ReadFloat();
	rotation->z = dispatch->ReadFloat();

	//actuall create shape
	DynamicShape* shape = new OgreDynamicShape(this,index, origin,velocity,rotation,"sinbad.mesh");

	//for scale
	Vector3D v;
	v.x = 30;
	v.y = 30;
	v.z = 30;
	shape->scale(v);
	
	//are this the avatar?
	if(local)
	{
		mClient->mShape = shape;	
		LogString("call SendReq");
	}

	//need to know about world
	shape->mGame = this;

	//ghost
	DynamicShape* ghostShape = new OgreDynamicShape(this,index,origin,velocity,rotation,"sinbad.mesh");
	
	shape->mGhost = ghostShape;

	//scale ghost set visible
	ghostShape->scale(v);
	ghostShape->setVisible(true);

	//now add to vectors....
	mShapeVector.push_back(shape);
	mShapeGhostVector.push_back(shape->mGhost);
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

void Game::createScene(void)
{
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.75, 0.75, 0.75));

    Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(Ogre::Vector3(250, 150, 250));
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);

}

 void Game::shutdown(void)
 {
	if(!mInit)
		return;

	LogString("Game::Disconnect");

	mInit = false;

	mClient->sendDisconnect();
 }

//this function should simply move ghost directly to latest server info, in this case mServerFrame is set in ReadDeltaMove
//unless it did not change on server in which case it should contain same value.
void Game::moveGhostShapes(DynamicShape* shape)
{
	Vector3D transVector;

	transVector.x = shape->mServerFrame.mOrigin.x;
	transVector.y = 0;
	transVector.z = shape->mServerFrame.mOrigin.z;

	if (shape->mGhost)
	{
		shape->mGhost->setPosition(transVector);
	}
}
void Game::readPackets()
{
	int type;
	int ret;

	Dispatch* dispatch = new Dispatch(mClient->mSizeOfDispatch);

	while(ret = mClient->getPacket(dispatch))
	{
		dispatch->SetSize(ret);
		dispatch->BeginReading();

		type = dispatch->ReadByte();

		switch(type)
		{
			case mClient->mMessageAddShape:
				addShape(dispatch);
			break;

			case mClient->mMessageRemoveShape:
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

void Game::frame(Dispatch* dispatch)
{
	int newTime;
	int time;

	// Skip sequences
	dispatch->ReadShort();
	dispatch->ReadShort();

	newTime = dreamSock_GetCurrentSystemTime();
	time = newTime - mOldTime;
    mOldTime = newTime;

	//for (unsigned int i = 0; i < mShapeVector.size(); i++)
	while (dispatch->getReadCount() <= dispatch->GetSize())
	{
		if (mDatagramSocketShutdown)
		{
			return;
		}
		//readDeltaMoveCommand(dispatch);
		DynamicShape* shape = NULL;

		mDetailsPanel->setParamValue(11, Ogre::StringConverter::toString(dispatch->GetSize()));

		//index
		int id = dispatch->ReadByte();

		shape = getDynamicShape(id);

		if (shape)
		{
			shape->readDeltaMoveCommand(dispatch);
		}
	}
}

//this is all shapes coming to client game from server
//should a shape be responsible to read it's own command?????
//once we determine it's about him shouldn't we pass it off to
//shape object to handle?
void Game::readDeltaMoveCommand(Dispatch* mes)
{
	DynamicShape* shape = NULL;

	mDetailsPanel->setParamValue(11, Ogre::StringConverter::toString(mes->GetSize()));

	//index
	int id = mes->ReadByte();

	shape = getDynamicShape(id);

	if (shape)
	{
		shape->readDeltaMoveCommand(mes);
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

//this the client's (in this case we are on clientside so there is only one client instance) move being built
//to send to the server, all we are sending is a key(maybe) and always milliseconds.





void Game::processUnbufferedInput()
{
	mClient->mClientCommandToServer.mKey = 0;
    
	if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) // ESCAPE
    {
		mDatagramSocketShutdown = true;
    }
	else
	{
		mDatagramSocketShutdown = false;
	}
    	
	if (mKeyboard->isKeyDown(OIS::KC_I)) // Forward
    {
		mClient->mClientCommandToServer.mKey |= mKeyUp;
    }

    if (mKeyboard->isKeyDown(OIS::KC_K)) // Backward
    {
		mClient->mClientCommandToServer.mKey |= mKeyDown;
    }

	if (mKeyboard->isKeyDown(OIS::KC_J)) // Left - yaw or strafe
    {
		mClient->mClientCommandToServer.mKey |= mKeyLeft;
    }

    if (mKeyboard->isKeyDown(OIS::KC_L)) // Right - yaw or strafe
    {
		mClient->mClientCommandToServer.mKey |= mKeyRight;
    }

	mClient->mClientCommandToServer.mMilliseconds = (int) (mFrameTime * 1000);
}

void Game::buttonHit(OgreBites::Button *button)
{
	if (button == mJoinButton)
	{
		mJoinGame = true;
		if (mJoinGame && !mPlayingGame)
		{
			mClient->sendConnect("myname");
			LogString("sent connect to server");
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

bool Game::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	mRenderTime = evt.timeSinceLastFrame;

    bool ret = BaseApplication::frameRenderingQueued(evt);
	
	return ret;
}

void Game::runNetwork(int msec)
{
	static int time = 0;
	time += msec;

	readPackets();
	
	// Framerate is too high
	if(time > (1000 / 60))
	{
		mClient->sendCommand();
		mFrameTime = time / 1000.0f;
		time = 0;
	}
}

void Game::gameLoop()
{
	while(true)
    {

		processUnbufferedInput();

		if(game != NULL)
		{
			game->runNetwork(mRenderTime * 1000);
			interpolateFrame();
		}



		if (!runGraphics())
		{
			break;
		}

		initializeGui();
	}
}

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

void Game::interpolateFrame()
{
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		mShapeVector.at(i)->interpolateTick(mRenderTime);
	}
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

int Game::dreamSock_GetCurrentSystemTime(void)
{
#ifndef WIN32
	return mDreamLinuxSock->dreamSock_Linux_GetCurrentSystemTime();
#else
	return mDreamWinSock->dreamSock_Win_GetCurrentSystemTime();
#endif
}


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
        game = new Game(strCmdLine);
#else
        game = new Game(argv[1]);
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

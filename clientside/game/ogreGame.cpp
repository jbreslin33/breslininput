#include "ogreGame.h"
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


OgreGame* game;
//bool keys[256];

OgreGame::OgreGame(const char* serverIP) : Game(serverIP)
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

	
 }

OgreGame::~OgreGame()
{
	delete mClient;
}



DynamicShape* OgreGame::AddShape(Game* game,int local, int ind, char *name, float originX, float originY, float originZ,
					float velocityX, float velocityY, float velocityZ, float rotationX, float rotationZ)
{
	Vector3D* position = new Vector3D();
	position->x = originX;
	position->y = originY;
	position->z = originZ;

	Vector3D* velocity = new Vector3D();
	velocity->x = velocityX;
	velocity->y = velocityY;
	velocity->z = velocityZ;

	Vector3D* rotation = new Vector3D();
	rotation->x = rotationX;
	rotation->y = 0;
	rotation->z = rotationZ;


	DynamicShape* shape = new OgreDynamicShape(this,ind, position,velocity,rotation,"sinbad.mesh");
	Vector3D v;
	v.x = 30;
	v.y = 30;
	v.z = 30;
	shape->scale(v);
	
	if(local)
	{
		mClient->mShape = shape;	
		LogString("call SendReq");
	}

	shape->mGame = this;
	shape->mGhost = AddGhostShape(game,ind,position,velocity,rotation);
	return shape;
}

DynamicShape* OgreGame::AddGhostShape(Game* game, int ind,Vector3D* position, Vector3D* velocity, Vector3D* rotation)
{
	DynamicShape* shape = new OgreDynamicShape(game,ind,position,velocity,rotation,"sinbad.mesh");
	Vector3D v;
	v.x = 30;
	v.y = 30;
	v.z = 30;
	shape->scale(v);
	shape->setVisible(true);
	return shape;
}

void OgreGame::RemoveShape(int index)
{
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		if (mShapeVector.at(i)->mIndex == index)
		{
			delete mShapeVector.at(i);
			mShapeVector.erase (mShapeVector.begin()+i);
		}
	}
}

void OgreGame::createScene(void)
{
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.75, 0.75, 0.75));

    Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(Ogre::Vector3(250, 150, 250));
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);

}

 void OgreGame::Shutdown(void)
 {
 	Disconnect();
 }

void OgreGame::CheckKeys(void)
{

	if (mClient->mShape) 
	{
		mClient->mClientCommandToServer.mKey = 0;
 		if(keys[VK_ESCAPE])
 		{
			mDatagramSocketShutdown = true;
 		}
	
 		if(keys[VK_DOWN])
 		{
 			mClient->mClientCommandToServer.mKey |= KEY_DOWN;
 		}
 		if(keys[VK_UP])
 		{
 			mClient->mClientCommandToServer.mKey |= KEY_UP;
 		}
 		if(keys[VK_LEFT])
 		{
 			mClient->mClientCommandToServer.mKey |= KEY_LEFT;
 		}
 		if(keys[VK_RIGHT])
 		{
 			mClient->mClientCommandToServer.mKey |= KEY_RIGHT;
 		}
		if(keys[VK_SPACE])
		{
	 		//mClient->mClientCommandToServer.mKey |= KEY_SPACE;
	 	}
	 	mClient->mClientCommandToServer.mMilliseconds = (int) (mFrameTime * 1000);
	}
}

//this function should simply move ghost directly to latest server info, in this case mServerFrame is set in ReadDeltaMove
//unless it did not change on server in which case it should contain same value.
void OgreGame::moveGhostShapes(DynamicShape* shape)
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
void OgreGame::ReadPackets(void)
{
//	char data[1400];

	int type;
	int ind;
	int local;
	Vector3D origin;
	Vector3D velocity; 
	Vector3D rotation;
	int ret;
	int newTime;
	int time;

DynamicShape* shape;

	char name[50];

	Dispatch* dispatch = new Dispatch(mClient->mSizeOfDispatch);

	while(ret = mClient->getPacket(dispatch))
	{
		dispatch->SetSize(ret);
		dispatch->BeginReading();

		type = dispatch->ReadByte();

		switch(type)
		{
		case mClient->mMessageAddShape:
			local	= dispatch->ReadByte();
			ind		= dispatch->ReadByte();
			strcpy(name, dispatch->ReadString());
			origin.x = dispatch->ReadFloat();
			origin.y = dispatch->ReadFloat();
			origin.z = dispatch->ReadFloat();
			velocity.x = dispatch->ReadFloat();
			velocity.y = dispatch->ReadFloat();
			velocity.z = dispatch->ReadFloat();
			rotation.x = dispatch->ReadFloat();
			rotation.z = dispatch->ReadFloat();
			shape = AddShape(this,local, ind, name,origin.x,origin.y,origin.z,velocity.x,velocity.y,velocity.z,rotation.x,rotation.z);

			//now add to vectors....
			mShapeVector.push_back(shape);
			mShapeGhostVector.push_back(shape->mGhost);

			break;

		case mClient->mMessageRemoveShape:
			ind = dispatch->ReadByte();
			RemoveShape(ind);

			break;

		case USER_MES_FRAME:
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
				ReadDeltaMoveCommand(dispatch);
			}
			break;

		case USER_MES_SERVEREXIT:
			Disconnect();
			break;

		}
	}
}

void OgreGame::SendCommand(void)
{
	if(mClient->mConnectionState != mClient->mMessageConnected)
		return;

	int outgoingSequence = mClient->mOutgoingSequence & (COMMAND_HISTORY_SIZE-1);

	Dispatch* dispatch = new Dispatch(1400);

	dispatch->WriteByte(USER_MES_FRAME);						// type
	dispatch->WriteShort(mClient->mOutgoingSequence);
	dispatch->WriteShort(mClient->mIncomingSequence);

	// Build delta-compressed move command
	BuildDeltaMoveCommand(dispatch);

	// Send the packet
	mClient->sendPacket(dispatch);

	// Store the command to the input client's history
	memcpy(&mClient->mClientCommandToServerArray[outgoingSequence], &mClient->mClientCommandToServer, sizeof(Command));

}

void OgreGame::Disconnect(void)
{

	if(!mInit)
		return;

	LogString("OgreGame::Disconnect");

	mInit = false;

	mClient->sendDisconnect();
}

//this is all shapes coming to client game from server
//should a shape be responsible to read it's own command?????
//once we determine it's about him shouldn't we pass it off to
//shape object to handle?
void OgreGame::ReadDeltaMoveCommand(Dispatch* mes)
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

DynamicShape* OgreGame::getDynamicShape(int id)
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



void OgreGame::BuildDeltaMoveCommand(Dispatch* dispatch)
{
	int flags = 0;
	int last = (mClient->mOutgoingSequence - 1) & (COMMAND_HISTORY_SIZE-1);

	// Check what needs to be updated
	if(mClient->mClientCommandToServerArray[last].mKey != mClient->mClientCommandToServer.mKey)
	{
		flags |= CMD_KEY;
	}

	if(mClient->mClientCommandToServerArray[last].mMilliseconds != mClient->mClientCommandToServer.mMilliseconds)
	{
		flags |= CMD_MILLISECONDS;
	}

	// Add to the message
	
	//Flags
	dispatch->WriteByte(flags);

	// Key
	if(flags & CMD_KEY)
	{
		dispatch->WriteByte(mClient->mClientCommandToServer.mKey);
	}

	if(flags & CMD_MILLISECONDS)
	{
		dispatch->WriteByte(mClient->mClientCommandToServer.mMilliseconds);
	}
}



void OgreGame::processUnbufferedInput()
{
    if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) // ESCAPE
    {
		keys[VK_ESCAPE] = TRUE;
    }
	else
	{
		keys[VK_ESCAPE] = FALSE;
	}
    if (mKeyboard->isKeyDown(OIS::KC_I)) // Forward
    {
		keys[VK_UP] = TRUE;
    }
	else
	{
        keys[VK_UP] = FALSE;
	}
    if (mKeyboard->isKeyDown(OIS::KC_K)) // Backward
    {
		keys[VK_DOWN] = TRUE;
    }
	else
	{
        keys[VK_DOWN] = FALSE;
	}
    if (mKeyboard->isKeyDown(OIS::KC_J)) // Left - yaw or strafe
    {
		keys[VK_LEFT] = TRUE;
    }
	else
	{
        keys[VK_LEFT] = FALSE;
	}
    if (mKeyboard->isKeyDown(OIS::KC_L)) // Right - yaw or strafe
    {
		keys[VK_RIGHT] = TRUE;
    }
	else
	{
        keys[VK_RIGHT] = FALSE;
	}

}

void OgreGame::buttonHit(OgreBites::Button *button)
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

bool OgreGame::mouseMoved( const OIS::MouseEvent &arg )
{
    if (mTrayMgr->injectMouseMove(arg)) return true;
	if (mPlayingGame)
	{
		mCameraMan->injectMouseMove(arg);
	}
    return true;
}

bool OgreGame::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	mRenderTime = evt.timeSinceLastFrame;

    bool ret = BaseApplication::frameRenderingQueued(evt);
	
	return ret;
}

void OgreGame::RunNetwork(int msec)
{
	static int time = 0;
	time += msec;

	ReadPackets();
	
	// Framerate is too high
	if(time > (1000 / 60)) {

		//I think i want to official grab the key tally here along with the milliseconds tally.

		SendCommand();
		mFrameTime = time / 1000.0f;
		time = 0;
	}
}

void OgreGame::gameLoop()
{
	while(true)
    {

		processUnbufferedInput();
		if(game != NULL)
		{
			game->CheckKeys();
			game->RunNetwork(mRenderTime * 1000);
		}

		checkForShutdown();
		interpolateFrame();

		if (!runGraphics())
		{
			break;
		}

		initializeGui();

	}
}

void OgreGame::initializeGui()
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

void OgreGame::loadJoinScreen()
{
	unloadOtherScreens();
	mJoinButton = mTrayMgr->createButton(OgreBites::TL_CENTER, "mJoinButton", "Join Game");
	mTrayMgr->moveWidgetToTray(mJoinButton,OgreBites::TL_CENTER);
	mTrayMgr->showCursor();
}

void OgreGame::hideGui()
{
	hideJoinScreen();
	mTrayMgr->hideCursor();
}

void OgreGame::hideJoinScreen()
{
	mTrayMgr->removeWidgetFromTray(mJoinButton);
    mJoinButton->hide();
}

void OgreGame::unloadOtherScreens()
{

}

void OgreGame::interpolateFrame()
{
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		mShapeVector.at(i)->interpolateTick(mRenderTime);
	}
}

void OgreGame::checkForShutdown()
{
	if (mDatagramSocketShutdown == true)
	{
		mShutDown = true;
	}
}

bool OgreGame::runGraphics()
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

int OgreGame::dreamSock_GetCurrentSystemTime(void)
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
        game = new OgreGame(strCmdLine);
#else
        game = new OgreGame(argv[1]);
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

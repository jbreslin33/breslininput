#include "game.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../clientside/network/network.h"

#include "../../clientside/shape/ogreDynamicShape.h"

#include "../../clientside/client/client.h"

#include "../../math/vector3D.h"

#include "../message/message.h"

Game* game;
bool keys[256];

Game::Game(const char* serverIP)
{
	StartLog();

	mServerIP = serverIP;
 	mClient	= new Client("", mServerIP, 30004);

	mFrameTime		 = 0.0f;
 	mRenderTime		 = 0.0f;
	mOldTime         = 0;
 	mInit			 = false;
	mNetworkShutdown = false;

	mInit = true;

	mJoinGame    = false;
	mPlayingGame = false;
	mInitializeGui = false;

	
 }

Game::~Game()
{
	delete mClient;
}



DynamicShape* Game::AddShape(Game* game,int local, int ind, char *name, float originX, float originY, float originZ,
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

DynamicShape* Game::AddGhostShape(Game* game, int ind,Vector3D* position, Vector3D* velocity, Vector3D* rotation)
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

void Game::RemoveShape(int index)
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

void Game::createScene(void)
{
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.75, 0.75, 0.75));

    Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(Ogre::Vector3(250, 150, 250));
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);

}

 void Game::Shutdown(void)
 {
 	Disconnect();
 }

void Game::CheckKeys(void)
{

	if (mClient->mShape) 
	{
		mClient->mClientCommandToServer.mKey = 0;
 		if(keys[VK_ESCAPE])
 		{
			mNetworkShutdown = true;
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
void Game::ReadPackets(void)
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

	//Message mes;
	//message->Init(data, sizeof(data));
	char temp[1400];
	Message* message = new Message(mClient->mMessage->outgoingData,
		sizeof(mClient->mMessage->outgoingData));

	while(ret = mClient->GetPacket(message->data))
	{
		message->SetSize(ret);
		message->BeginReading();

		type = message->ReadByte();

		switch(type)
		{
		case mClient->mMessageAddShape:
			local	= message->ReadByte();
			ind		= message->ReadByte();
			strcpy(name, message->ReadString());
			origin.x = message->ReadFloat();
			origin.y = message->ReadFloat();
			origin.z = message->ReadFloat();
			velocity.x = message->ReadFloat();
			velocity.y = message->ReadFloat();
			velocity.z = message->ReadFloat();
			rotation.x = message->ReadFloat();
			rotation.z = message->ReadFloat();
			shape = AddShape(this,local, ind, name,origin.x,origin.y,origin.z,velocity.x,velocity.y,velocity.z,rotation.x,rotation.z);

			//now add to vectors....
			mShapeVector.push_back(shape);
			mShapeGhostVector.push_back(shape->mGhost);

			break;

		case mClient->mMessageRemoveShape:
			ind = message->ReadByte();
			RemoveShape(ind);

			break;

		case USER_MES_FRAME:
			// Skip sequences
			message->ReadShort();
			message->ReadShort();

			newTime = mClient->mNetwork->dreamSock_GetCurrentSystemTime();
			time = newTime - mOldTime;
            mOldTime = newTime;

			//for (unsigned int i = 0; i < mShapeVector.size(); i++)
			while (message->getReadCount() <= message->GetSize())
			{
				if (mNetworkShutdown)
				{
					return;
				}
				ReadDeltaMoveCommand(message);
			}
			break;

		case USER_MES_SERVEREXIT:
			Disconnect();
			break;

		}
	}
}

void Game::SendCommand(void)
{
	if(mClient->mConnectionState != mClient->mMessageConnected)
		return;

	//Message message;
	//char data[1400];

	int outgoingSequence = mClient->mOutgoingSequence & (COMMAND_HISTORY_SIZE-1);

//	message.Init(data, sizeof(data));
	Message* message = new Message(mClient->mTempDataBuffer,
		sizeof(mClient->mMessage->outgoingData));

	message->WriteByte(USER_MES_FRAME);						// type
	message->WriteShort(mClient->mOutgoingSequence);
	message->WriteShort(mClient->mIncomingSequence);

	// Build delta-compressed move command
	BuildDeltaMoveCommand(message);

	// Send the packet
	mClient->SendPacket(message);

	// Store the command to the input client's history
	memcpy(&mClient->mClientCommandToServerArray[outgoingSequence], &mClient->mClientCommandToServer, sizeof(Command));

}

void Game::Disconnect(void)
{

	if(!mInit)
		return;

	LogString("Game::Disconnect");

	mInit = false;

	mClient->SendDisconnect();
}

//this is all shapes coming to client game from server
//should a shape be responsible to read it's own command?????
//once we determine it's about him shouldn't we pass it off to
//shape object to handle?
void Game::ReadDeltaMoveCommand(Message *mes)
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



void Game::BuildDeltaMoveCommand(Message *mes)
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
	mes->WriteByte(flags);

	// Key
	if(flags & CMD_KEY)
	{
		mes->WriteByte(mClient->mClientCommandToServer.mKey);
	}

	if(flags & CMD_MILLISECONDS)
	{
		mes->WriteByte(mClient->mClientCommandToServer.mMilliseconds);
	}
}



void Game::processUnbufferedInput()
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

void Game::buttonHit(OgreBites::Button *button)
{
	if (button == mJoinButton)
	{
		mJoinGame = true;
		if (mJoinGame && !mPlayingGame)
		{
			mClient->SendConnect("myname");
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

void Game::RunNetwork(int msec)
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

void Game::gameLoop()
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

void Game::checkForShutdown()
{
	if (mNetworkShutdown == true)
	{
		mShutDown = true;
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

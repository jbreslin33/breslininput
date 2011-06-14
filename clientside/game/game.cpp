#include "game.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../network/network.h"

#include "../../clientside/shape/shape.h"

#include "../../client/client.h"

#include "../../math/vector3D.h"

#include "../../message/message.h"

Game* game;
bool keys[256];

Game::Game(const char* serverIP)
{
	StartLog();

	mServerIP = serverIP;
 	mClient	= new Client("", mServerIP, 30004);

	mFrameTime		= 0.0f;
 	mRenderTime		= 0.0f;
	mOldTime = 0;
 	mInit			= false;
	mNetworkShutdown = false;

	mInit = true;

	mClient->SendConnect("myname");
 }

Game::~Game()
{
	delete mClient;
}

void Game::AddShape(int local, int ind, char *name)
{
	Vector3D* pos = new Vector3D();
	pos->x = 0;
	pos->y = 0;
	pos->z = 0;

	Shape* shape = new Shape(pos,mSceneMgr,"sinbad.mesh");
	//shape->getSceneNode()->scale(30,30,30);
	
	shape->mIndex = ind;

	mShapeVector.push_back(shape);
	
	if(local)
	{
		mClient->mShape = shape;	
		LogString("call SendReq");
		SendRequestNonDeltaFrame();
	}

	shape->mGame = this;
	
	shape->mGhost = AddGhostShape(ind);
}

OgreShape* Game::AddGhostShape(int ind)
{
	Vector3D* pos = new Vector3D();
	pos->x = 0;
	pos->y = 0;
	pos->z = 0;

	Shape* shape = new Shape(pos,mSceneMgr,"sinbad.mesh");
	//shape->getSceneNode()->scale(30,30,30);
	
	shape->mIndex = ind;

	mShapeGhostVector.push_back(shape);
	shape->getSceneNode()->setVisible(false);
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
		mClient->mShape->mCommand.mKey = 0;
 		if(keys[VK_ESCAPE])
 		{
			mNetworkShutdown = true;
 		}
	
 		if(keys[VK_DOWN])
 		{
 			mClient->mShape->mCommand.mKey |= KEY_DOWN;
 		}
 		if(keys[VK_UP])
 		{
 			mClient->mShape->mCommand.mKey |= KEY_UP;
 		}
 		if(keys[VK_LEFT])
 		{
 			mClient->mShape->mCommand.mKey |= KEY_LEFT;
 		}
 		if(keys[VK_RIGHT])
 		{
 			mClient->mShape->mCommand.mKey |= KEY_RIGHT;
 		}
		if(keys[VK_SPACE])
		{
	 		mClient->mShape->mCommand.mKey |= KEY_SPACE;
	 	}

	 	mClient->mShape->mCommand.mMilliseconds = (int) (mFrameTime * 1000);
	}
	else
	{
		if(keys[VK_SPACE])
	 	{
			if (mClient->connectionState == DREAMSOCK_DISCONNECTED)
			{
				LogString("send a connect dude");
				mClient->SendConnect("myname");
				//for (int i = 1; i < 20; i++)
				//{
				//	AddShape(0,i,"a");
				//}
			}
		}
	}
}

void Game::moveGhostShapes()
{
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

		transVector.x = mShapeVector.at(i)->mServerFrame.mOrigin.x;
		transVector.z = mShapeVector.at(i)->mServerFrame.mOrigin.z;

		//if (mShapeGhostVector.at(i))
		//{
			mShapeVector.at(i)->mGhost->getSceneNode()->setPosition(transVector);
		//}
	}
}
void Game::ReadPackets(void)
{
	char data[1400];
	struct sockaddr address;

	int type;
	int ind;
	int local;
	int ret;
	int newTime;
	int time;

	char name[50];

	Message mes;
	mes.Init(data, sizeof(data));

	while(ret = mClient->GetPacket(mes.data, &address))
	{
		mes.SetSize(ret);
		mes.BeginReading();

		type = mes.ReadByte();

		switch(type)
		{
		case DREAMSOCK_MES_ADDSHAPE:
			local	= mes.ReadByte();
			ind		= mes.ReadByte();
			strcpy(name, mes.ReadString());
			AddShape(local, ind, name);
			break;

		case DREAMSOCK_MES_REMOVESHAPE:
			ind = mes.ReadByte();
			RemoveShape(ind);

			break;

		case USER_MES_FRAME:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			newTime = mClient->mNetwork->dreamSock_GetCurrentSystemTime();
			time = newTime - mOldTime;
            mOldTime = newTime;

			for (unsigned int i = 0; i < mShapeVector.size(); i++)
			{
				if (mNetworkShutdown)
					return;
				ReadDeltaMoveCommand(&mes, mShapeVector.at(i));
				mShapeVector.at(i)->processTick(); //when you read packets??

				moveGhostShapes(); //let the ghosts move since we just got the update..
			}
			break;

		case USER_MES_NONDELTAFRAME:
			// Skip sequences
			LogString("USER_MES_NONDELTAFRAME");
			//mes.ReadShort();
		//	mes.ReadShort();

			mOldTime = mClient->mNetwork->dreamSock_GetCurrentSystemTime();

			for (unsigned int i = 0; i < mShapeVector.size(); i++)
			{
				LogString("Reading NONDELTAFRAME for client %d", mShapeVector.at(i)->mIndex);
				ReadMoveCommand(&mes, mShapeVector.at(i));
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
	if(mClient->GetConnectionState() != DREAMSOCK_CONNECTED)
		return;

	Message message;
	char data[1400];

	int i = mClient->GetOutgoingSequence() & (COMMAND_HISTORY_SIZE-1);

	message.Init(data, sizeof(data));
	message.WriteByte(USER_MES_FRAME);						// type
	message.AddSequences(mClient);					// sequences

	// Build delta-compressed move command
	BuildDeltaMoveCommand(&message);

	// Send the packet
	mClient->SendPacket(&message);

	// Store the command to the input client's history
	memcpy(&mClient->mShape->mFrame[i], &mClient->mShape->mCommand, sizeof(Command));

	// Store the commands to the clients' history
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		memcpy(&mShapeVector.at(i)->mFrame[i], &mShapeVector.at(i)->mCommand, sizeof(Command));
	}
}

void Game::SendRequestNonDeltaFrame(void)
{
	LogString("sendReq");
	char data[1400];
	Message message;
	message.Init(data, sizeof(data));

	message.WriteByte(USER_MES_NONDELTAFRAME);
	message.AddSequences(mClient);

	mClient->SendPacket(&message);
}

void Game::Disconnect(void)
{

	if(!mInit)
		return;

	LogString("Game::Disconnect");

	mInit = false;

	mClient->SendDisconnect();
}

void Game::ReadMoveCommand(Message *mes, Shape *shape)
{
	// Key
	shape->mServerFrame.mKey			= mes->ReadByte();

	// Origin
	shape->mServerFrame.mOrigin.x		= mes->ReadFloat();
	shape->mServerFrame.mOrigin.z		= mes->ReadFloat();
	shape->mServerFrame.mVelocity.x		= mes->ReadFloat();
	shape->mServerFrame.mVelocity.z		= mes->ReadFloat();

	// Read time to run command
	shape->mServerFrame.mMilliseconds = mes->ReadByte();

	memcpy(&shape->mCommand, &shape->mServerFrame, sizeof(Command));

	// Fill the history array with the position we got
	for(int f = 0; f < COMMAND_HISTORY_SIZE; f++)
	{
		shape->mFrame[f].mPredictedOrigin.x = shape->mCommand.mOrigin.x;
		shape->mFrame[f].mPredictedOrigin.z = shape->mCommand.mOrigin.z;
	}
	LogString("read move command");
}

void Game::ReadDeltaMoveCommand(Message *mes, Shape *shape)
{
	//shape->mProcessedFrame;
	int flags = 0;

	// Flags
	flags = mes->ReadByte();

	// Key
	if(flags & CMD_KEY)
	{
		shape->mServerFrame.mKey = mes->ReadByte();
		shape->mCommand.mKey = shape->mServerFrame.mKey;
		//LogString("Client %d: Read key %d", shape->mIndex, shape->mCommand.mKey);
	}

	// Origin
	if(flags & CMD_ORIGIN)
	{
		shape->mProcessedFrame = mes->ReadByte();
	}

	// Read time to run command
	shape->mServerFrame.mOrigin.x = mes->ReadFloat();
	shape->mServerFrame.mOrigin.z = mes->ReadFloat();
	shape->mServerFrame.mOrigin.y = mes->ReadFloat();

	shape->mServerFrame.mVelocity.x = mes->ReadFloat();
	shape->mServerFrame.mVelocity.z = mes->ReadFloat();
	shape->mServerFrame.mVelocity.y = mes->ReadFloat();

	shape->mServerFrame.mRotOld.x = shape->mServerFrame.mRot.x;
	shape->mServerFrame.mRotOld.z = shape->mServerFrame.mRot.z;

	shape->mServerFrame.mRot.x = mes->ReadFloat();
	shape->mServerFrame.mRot.z = mes->ReadFloat();

	shape->mCommand.mMilliseconds = mes->ReadByte();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc:
//-----------------------------------------------------------------------------
void Game::BuildDeltaMoveCommand(Message *mes)
{
	int flags = 0;
	int last = (mClient->GetOutgoingSequence() - 1) & (COMMAND_HISTORY_SIZE-1);

	// Check what needs to be updated
	if(mClient->mShape->mFrame[last].mKey != mClient->mShape->mCommand.mKey)
		flags |= CMD_KEY;

	// Add to the message
	
	//Flags
	mes->WriteByte(flags);

	// Key
	if(flags & CMD_KEY)
	{
		mes->WriteByte(mClient->mShape->mCommand.mKey);
	}

	mes->WriteByte(mClient->mShape->mCommand.mMilliseconds);
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
	if (mKeyboard->isKeyDown(OIS::KC_SPACE)) // Right - yaw or strafe
    {
		keys[VK_SPACE] = TRUE;
    }
	else
	{
        keys[VK_SPACE] = FALSE;
	}
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
	
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		//mShapeVector.at(i)->mRenderTime = mRenderTime;
		mShapeVector.at(i)->interpolateTick(mRenderTime);
	}
	//if (mClient->connectionState == 1)
	//{
		// Framerate is too high
		if(time > (1000 / 60)) {
		
			SendCommand();
			mFrameTime = time / 1000.0f;
			time = 0;
		}
	//}
}

void Game::gameLoop()
{
	while(true)
    {
		processUnbufferedInput();
		if(game != NULL)
		{
			game->CheckKeys();

			if (mNetworkShutdown == true)
			{
				Shutdown();
			}
			game->RunNetwork(mRenderTime * 1000);
		}

		if (mNetworkShutdown == true)
		{
			mShutDown = true;
		}

		
		//Pump messages in all registered RenderWindow windows
		WindowEventUtilities::messagePump();

		if (!mRoot->renderOneFrame())
			break;
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

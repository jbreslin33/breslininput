#include "game.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../network/network.h"

#include "../../clientside/shape/shape.h"

#include "../../clientside/client/client.h"

#include "../../math/vector3D.h"

#include "../../message/message.h"

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
 }

Game::~Game()
{
	delete mClient;
}

Shape* Game::AddShape(int local, int ind, char *name, float originX, float originY, float originZ,
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
	rotation->z = rotationZ;
	rotation->y = 0;

	Shape* shape = new Shape(ind, position,velocity,rotation,mSceneMgr,"sinbad.mesh");
	shape->getSceneNode()->scale(30,30,30);
	
	if(local)
	{
		mClient->mShape = shape;	
		LogString("call SendReq");
	}

	shape->mGame = this;
	
	shape->mGhost = AddGhostShape(ind,position,velocity,rotation);

	return shape;
}

Shape* Game::AddGhostShape(int ind,Vector3D* position, Vector3D* velocity, Vector3D* rotation)
{
	Shape* shape = new Shape(ind,position,velocity,rotation,mSceneMgr,"sinbad.mesh");
	shape->getSceneNode()->scale(30,30,30);
	//shape->getSceneNode()->setVisible(false);
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
void Game::moveGhostShapes()
{
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

		transVector.x = mShapeVector.at(i)->mServerFrame.mOrigin.x;
		transVector.z = mShapeVector.at(i)->mServerFrame.mOrigin.z;

		if (mShapeVector.at(i)->mGhost)
		{
			mShapeVector.at(i)->mGhost->getSceneNode()->setPosition(transVector);
		}
	}
}
void Game::ReadPackets(void)
{
	char data[1400];
	struct sockaddr address;

	int type;
	int ind;
	int local;
	Vector3D origin;
	Vector3D velocity; 
	Vector3D rotation;
	int ret;
	int newTime;
	int time;

Shape* shape;

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
			origin.x = mes.ReadFloat();
			origin.y = mes.ReadFloat();
			origin.z = mes.ReadFloat();
			velocity.x = mes.ReadFloat();
			velocity.y = mes.ReadFloat();
			velocity.z = mes.ReadFloat();
			rotation.x = mes.ReadFloat();
			rotation.z = mes.ReadFloat();
			shape = AddShape(local, ind, name,origin.x,origin.y,origin.z,velocity.x,velocity.y,velocity.z,rotation.x,rotation.z);

			//now add to vectors....
			mShapeVector.push_back(shape);
			mShapeGhostVector.push_back(shape->mGhost);

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

		case USER_MES_SERVEREXIT:
			Disconnect();
			break;

		}
	}
}

void Game::SendCommand(void)
{
	if(mClient->mConnectionState != DREAMSOCK_CONNECTED)
		return;

	Message message;
	char data[1400];

	int outgoingSequence = mClient->mOutgoingSequence & (COMMAND_HISTORY_SIZE-1);

	message.Init(data, sizeof(data));
	message.WriteByte(USER_MES_FRAME);						// type
	message.WriteShort(mClient->mOutgoingSequence);
	message.WriteShort(mClient->mIncomingSequence);

	// Build delta-compressed move command
	BuildDeltaMoveCommand(&message);

	// Send the packet
	mClient->SendPacket(&message);

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
void Game::ReadDeltaMoveCommand(Message *mes, Shape *shape)
{
	int flags = 0;

	bool x = true;
	bool z = true;
	bool y = true;

	mDetailsPanel->setParamValue(11, Ogre::StringConverter::toString(mes->GetSize()));

	// Flags
	flags = mes->ReadByte();

	// Origin
	if(flags & CMD_ORIGIN_X)
	{
		shape->mServerFrame.mOriginOld.x = shape->mServerFrame.mOrigin.x;
		shape->mServerFrame.mOrigin.x = mes->ReadFloat();		
	}
	else
	{
		x = false;
	}

	if(flags & CMD_ORIGIN_Y)
	{
		shape->mServerFrame.mOriginOld.y = shape->mServerFrame.mOrigin.y;
		shape->mServerFrame.mOrigin.y = mes->ReadFloat();
	}
	else
	{
		y = false;
	}

	if(flags & CMD_ORIGIN_Z)
	{
		shape->mServerFrame.mOriginOld.z = shape->mServerFrame.mOrigin.z;
		shape->mServerFrame.mOrigin.z = mes->ReadFloat();	
	}
	else
	{
		z = false;
	}

	//set old rot
	shape->mServerFrame.mRotOld.x = shape->mServerFrame.mRot.x;
	shape->mServerFrame.mRotOld.z = shape->mServerFrame.mRot.z;

	//rotation
	if(flags & CMD_ROTATION_X)
	{
		shape->mServerFrame.mRot.x = mes->ReadFloat();
	}

	if(flags & CMD_ROTATION_Z)
	{
		shape->mServerFrame.mRot.z = mes->ReadFloat();
	}

	//milliseconds
	if (flags & CMD_MILLISECONDS)
	{
		shape->mServerFrame.mMilliseconds = mes->ReadByte();
		shape->mCommandToRunOnShape.mMilliseconds = shape->mServerFrame.mMilliseconds;
	}

	if (shape->mServerFrame.mMilliseconds != 0) 
	{
		if(!x && !z && !y && shape->mServerFrame.mMilliseconds != 0)
		{
			shape->mServerFrame.mVelocity.x = 0.0;
			shape->mServerFrame.mVelocity.y = 0.0;
			shape->mServerFrame.mVelocity.z = 0.0;
		}
		else
		{
			shape->mServerFrame.mVelocity.x = shape->mServerFrame.mOrigin.x - shape->mServerFrame.mOriginOld.x;
			shape->mServerFrame.mVelocity.y = shape->mServerFrame.mOrigin.y - shape->mServerFrame.mOriginOld.y;
			shape->mServerFrame.mVelocity.z = shape->mServerFrame.mOrigin.z - shape->mServerFrame.mOriginOld.z;
		}
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
	/*
	if (mKeyboard->isKeyDown(OIS::KC_SPACE)) // Right - yaw or strafe
    {
		keys[VK_SPACE] = TRUE;
    }
	else
	{
        keys[VK_SPACE] = FALSE;
	}
*/
	if (mKeyboard->isKeyDown(OIS::KC_V)) // Right - yaw or strafe
    {
		mJoinGame = TRUE;
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
		//if (mJoinGame && !mPlayingGame)
		//{
			//mClient->SendConnect("myname");
		//	mPlayingGame = true;
		//}
		processUnbufferedInput();
		if(game != NULL)
		{
			game->CheckKeys();
			game->RunNetwork(mRenderTime * 1000);
		}

		checkForShutdown();
		interpolateFrame();
		if (!runGraphics())
			break;

		if (!mPlayingGame)
		{
			mClient->SendConnect("myname");
			mPlayingGame = true;
		}
		
	}
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

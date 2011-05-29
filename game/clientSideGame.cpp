#include "clientSideGame.h"
#include "../tdreamsock/dreamSockLog.h"

#include "../network/network.h"

#include "../shape/clientSideShape.h"

#include "../client/client.h"

#include "../shape/ogreShape.h"
#include "../math/vector3D.h"

#include "../message/message.h"

ClientSideGame* game;
bool keys[256];

ClientSideGame::ClientSideGame(const char* serverIP)
{
	StartLog();

	mServerIP = serverIP;
 	mClient	= new Client("", mServerIP, 30004);

	mLocalShape		= NULL;
 	mInputShape        = NULL;

	mFrameTime		= 0.0f;
 	mRenderTime		= 0.0f;
	mOldTime = 0;
 	mInit			= false;
	mNetworkShutdown = false;

	mInit = true;

	mClient->SendConnect("myname");

 }

ClientSideGame::~ClientSideGame()
{


	delete mClient;
}

void ClientSideGame::AddPlayer(int local, int ind, char *name)
{
	ClientSideShape* clientSideShape = new ClientSideShape("jay" + ind,new Vector3D(),mSceneMgr,"sinbad.mesh");
	//mClient->mClientSideShape = new ClientSideShape("jay" + ind,new Vector3D(),mSceneMgr,"sinbad.mesh");
	clientSideShape->getSceneNode()->scale(30,30,30);
	
	clientSideShape->mIndex = ind;

	clientSideShape->mServerShape = new ClientSideShape("silentBob" + ind,new Vector3D(),mSceneMgr,"sinbad.mesh");
	clientSideShape->setServerShape(clientSideShape->mServerShape);
	clientSideShape->mServerShape->getSceneNode()->scale(30,30,30);
	clientSideShape->mServerShape->getSceneNode()->setVisible(false);
	mShapeVector.push_back(clientSideShape);
	
	if(local)
	{
		mLocalShape = clientSideShape;
		mInputShape = clientSideShape;
		
		SendRequestNonDeltaFrame();
	}
}

void ClientSideGame::createScene(void)
{
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.75, 0.75, 0.75));

    Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(Ogre::Vector3(250, 150, 250));
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
}

bool ClientSideGame::processUnbufferedInput(const Ogre::FrameEvent& evt)
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

    return true;
}

bool ClientSideGame::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(!processUnbufferedInput(evt)) return false;

	if(game != NULL)
	{
		if (mShapeVector.size() > 0)
		{
			//LogString("good");
			game->CheckKeys();
		}
		if (mNetworkShutdown == true)
		{
			Shutdown();
		}
		game->RunNetwork(evt.timeSinceLastFrame * 1000);
		mRenderTime = evt.timeSinceLastFrame;

	}
	if (mNetworkShutdown == true)
	{
		mShutDown = true;
	}

    bool ret = BaseApplication::frameRenderingQueued(evt);
	
	return ret;
}

 void ClientSideGame::Shutdown(void)
 {
 	Disconnect();
 }

void ClientSideGame::CheckKeys(void)
{
	mInputShape->mCommand.mKey = 0;
 	if(keys[VK_ESCAPE])
 	{
		LogString("esc");
		mNetworkShutdown = true;
 	}
	
 	if(keys[VK_DOWN])
 	{
 		mInputShape->mCommand.mKey |= KEY_DOWN;
 	}
 	if(keys[VK_UP])
 	{
 		mInputShape->mCommand.mKey |= KEY_UP;
 	}
 	if(keys[VK_LEFT])
 	{
 		mInputShape->mCommand.mKey |= KEY_LEFT;
 	}
 	if(keys[VK_RIGHT])
 	{
 		mInputShape->mCommand.mKey |= KEY_RIGHT;
 	}
	if(keys[VK_SPACE])
 	{
 		mInputShape->mCommand.mKey |= KEY_SPACE;
 	}

 	mInputShape->mCommand.mMilliseconds = (int) (mFrameTime * 1000);
 }

void ClientSideGame::MoveServerPlayer(void)
{
    Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

	transVector.x = mLocalShape->mServerFrame.mOrigin.x;
	transVector.z = mLocalShape->mServerFrame.mOrigin.z;

	if (mLocalShape->mServerShape)
	{
		mLocalShape->mServerShape->getSceneNode()->setPosition(transVector);
	}
}

void ClientSideGame::ReadPackets(void)
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
		case DREAMSOCK_MES_ADDCLIENT:
			local	= mes.ReadByte();
			ind		= mes.ReadByte();
			strcpy(name, mes.ReadString());
			AddPlayer(local, ind, name);
			break;

		case DREAMSOCK_MES_REMOVECLIENT:
			ind = mes.ReadByte();

			LogString("Got removeclient %d message", ind);
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
				mShapeVector.at(i)->processTick();

				MoveServerPlayer();
			}
			break;

		case USER_MES_NONDELTAFRAME:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			mOldTime = mClient->mNetwork->dreamSock_GetCurrentSystemTime();

			for (unsigned int i = 0; i < mShapeVector.size(); i++)
			{
				//LogString("Reading NONDELTAFRAME for client %d", mShapeVector.at(i)->mIndex);
				ReadMoveCommand(&mes, mShapeVector.at(i));
			}

			break;

		case USER_MES_SERVEREXIT:
			Disconnect();
			break;

		}
	}
}

void ClientSideGame::SendCommand(void)
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
	BuildDeltaMoveCommand(&message, mInputShape);

	// Send the packet
	mClient->SendPacket(&message);

	// Store the command to the input client's history
	memcpy(&mInputShape->mFrame[i], &mInputShape->mCommand, sizeof(Command));

	// Store the commands to the clients' history
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		memcpy(&mShapeVector.at(i)->mFrame[i], &mShapeVector.at(i)->mCommand, sizeof(Command));
	}
}

void ClientSideGame::SendRequestNonDeltaFrame(void)
{
	char data[1400];
	Message message;
	message.Init(data, sizeof(data));

	message.WriteByte(USER_MES_NONDELTAFRAME);
	message.AddSequences(mClient);

	mClient->SendPacket(&message);
}

void ClientSideGame::Disconnect(void)
{

	if(!mInit)
		return;

	LogString("ClientSideGame::Disconnect");

	mInit = false;
	mLocalShape = NULL;
	mInputShape = NULL;

	mClient->SendDisconnect();
}

void ClientSideGame::ReadMoveCommand(Message *mes, ClientSideShape *client)
{
	// Key
	client->mServerFrame.mKey			= mes->ReadByte();

	// Origin
	client->mServerFrame.mOrigin.x		= mes->ReadFloat();
	client->mServerFrame.mOrigin.z		= mes->ReadFloat();
	client->mServerFrame.mVelocity.x		= mes->ReadFloat();
	client->mServerFrame.mVelocity.z		= mes->ReadFloat();

	// Read time to run command
	client->mServerFrame.mMilliseconds = mes->ReadByte();

	memcpy(&client->mCommand, &client->mServerFrame, sizeof(Command));

	// Fill the history array with the position we got
	for(int f = 0; f < COMMAND_HISTORY_SIZE; f++)
	{
		client->mFrame[f].mPredictedOrigin.x = client->mCommand.mOrigin.x;
		client->mFrame[f].mPredictedOrigin.z = client->mCommand.mOrigin.z;
	}
}

void ClientSideGame::ReadDeltaMoveCommand(Message *mes, ClientSideShape *client)
{
	client->mProcessedFrame;
	int flags = 0;

	// Flags
	flags = mes->ReadByte();

	// Key
	if(flags & CMD_KEY)
	{
		client->mServerFrame.mKey = mes->ReadByte();

		client->mCommand.mKey = client->mServerFrame.mKey;
		LogString("Client %d: Read key %d", client->mIndex, client->mCommand.mKey);
	}

	// Origin
	if(flags & CMD_ORIGIN)
	{
		client->mProcessedFrame = mes->ReadByte();
	}

	// Read time to run command
	client->mServerFrame.mOrigin.x = mes->ReadFloat();
	client->mServerFrame.mOrigin.z = mes->ReadFloat();
	client->mServerFrame.mOrigin.y = mes->ReadFloat();

	client->mServerFrame.mVelocity.x = mes->ReadFloat();
	client->mServerFrame.mVelocity.z = mes->ReadFloat();
	client->mServerFrame.mVelocity.y = mes->ReadFloat();

	//LogString("x %f", client->mServerFrame.mVelocity.x);
    //LogString("z %f", client->mServerFrame.mVelocity.z);
    //LogString("y %f", client->mServerFrame.mVelocity.y);

	client->mServerFrame.mRotOld.x = client->mServerFrame.mRot.x;
	client->mServerFrame.mRotOld.z = client->mServerFrame.mRot.z;

	client->mServerFrame.mRot.x = mes->ReadFloat();
	client->mServerFrame.mRot.z = mes->ReadFloat();

	client->mCommand.mMilliseconds = mes->ReadByte();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc:
//-----------------------------------------------------------------------------
void ClientSideGame::BuildDeltaMoveCommand(Message *mes, ClientSideShape *theClient)
{
	int flags = 0;
	int last = (mClient->GetOutgoingSequence() - 1) & (COMMAND_HISTORY_SIZE-1);

	// Check what needs to be updated
	if(theClient->mFrame[last].mKey != theClient->mCommand.mKey)
		flags |= CMD_KEY;

	// Add to the message
	// Flags
	mes->WriteByte(flags);

	// Key
	if(flags & CMD_KEY)
	{
		mes->WriteByte(theClient->mCommand.mKey);
	}

	mes->WriteByte(theClient->mCommand.mMilliseconds);
}

void ClientSideGame::RunNetwork(int msec)
{
	static int time = 0;
	time += msec;

	ReadPackets();
	
	for (unsigned int i = 0; i < mShapeVector.size(); i++)
	{
		mShapeVector.at(i)->interpolateTick(mRenderTime);
	}

	// Framerate is too high
	if(time > (1000 / 60)) {
		
		SendCommand();
		mFrameTime = time / 1000.0f;
	    time = 0;
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

        //ClientSideBaseGame* mClientSideBaseGame;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		StartLogConsole();
        game = new ClientSideGame(strCmdLine);
#else
        game = new ClientSideGame(argv[1]);
#endif

        try {
            game->go();
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

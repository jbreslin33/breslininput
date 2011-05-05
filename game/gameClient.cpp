#include "gameClient.h"
#include "../player/clientSidePlayer.h"

#include "../shape/ogreShape.h"
#include "../math/vector3D.h"

GameClient* game;
bool keys[256];

GameClient::GameClient(const char* serverIP)
{
	mServerIP = serverIP;
 	mNetworkClient	= new dreamClient;
 	mLocalClient		= NULL;
 	memset(&mInputClient, 0, sizeof(ClientSideClient));
 	mFrametime		= 0.0f;
 	mRendertime		= 0.0f;
 	mInit			= false;
	mOldTime = 0;
 }

GameClient::~GameClient()
{
	delete mNetworkClient;
}

void GameClient::createPlayer(ClientSideClient* client, int index)
{
	
	//ClientSidePlayer* clientSidePlayer = new ClientSidePlayer(client,mSceneMgr,"jay" + index,0,0,0,"sinbad.mesh");
	OgreShape* shape = new OgreShape("jay" + index,new Vector3D(),mSceneMgr,"sinbad.mesh");
	
	ClientSidePlayer* clientSidePlayer = new ClientSidePlayer("jay" + index,client,shape);
	shape->getSceneNode()->scale(30,30,30);
	client->mPlayer = clientSidePlayer;
	client->mIndex = index;
}

void GameClient::AddClient(int local, int ind, char *name)
{
	ClientSideClient* clientSideClient = new ClientSideClient();
	mClientVector.push_back(clientSideClient);
	createPlayer(clientSideClient, ind);

	if(local)
	{
		mLocalClient = clientSideClient;
		createServerPlayer(clientSideClient,ind);
		SendRequestNonDeltaFrame();
	}
}

void GameClient::createServerPlayer(ClientSideClient* client, int index)
{
	OgreShape* shape = new OgreShape("silentBob" + index,new Vector3D(),mSceneMgr,"sinbad.mesh");
	ClientSidePlayer* clientSidePlayer = new ClientSidePlayer("silentBob" + index,client,shape);
  	shape->getSceneNode()->scale(30,30,30);
	client->mServerPlayer = clientSidePlayer;
}

void GameClient::createScene(void)
{
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.75, 0.75, 0.75));

    Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(Ogre::Vector3(250, 150, 250));
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
}

bool GameClient::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
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

    return true;
}

bool GameClient::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = BaseApplication::frameRenderingQueued(evt);

    if(!processUnbufferedInput(evt)) return false;

	if(game != NULL)
	{
		game->CheckKeys();
		game->RunNetwork(evt.timeSinceLastFrame * 1000);
		mRendertime = evt.timeSinceLastFrame;

	}
    return ret;
}

 void GameClient::Shutdown(void)
 {
 	Disconnect();
 }

void GameClient::CheckKeys(void)
{
	mInputClient.mCommand.mKey = 0;
 	if(keys[VK_ESCAPE])
 	{
 		Shutdown();
 		keys[VK_ESCAPE] = false;
 	}
 	if(keys[VK_DOWN])
 	{
 		mInputClient.mCommand.mKey |= KEY_DOWN;
 	}
 	if(keys[VK_UP])
 	{
 		mInputClient.mCommand.mKey |= KEY_UP;
 	}
 	if(keys[VK_LEFT])
 	{
 		mInputClient.mCommand.mKey |= KEY_LEFT;
 	}
 	if(keys[VK_RIGHT])
 	{
 		mInputClient.mCommand.mKey |= KEY_RIGHT;
 	}
 	mInputClient.mCommand.mMilliseconds = (int) (mFrametime * 1000);
 }

void GameClient::MoveServerPlayer(void)
{
    Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

	transVector.x = mLocalClient->mServerFrame.mOrigin.x;
	transVector.z = -mLocalClient->mServerFrame.mOrigin.z;

	if (mLocalClient->mServerPlayer)
	{
		//LogString("try to move server player");
			mLocalClient->mServerPlayer->mShape->getSceneNode()->setPosition(transVector);
	}
}

void GameClient::StartConnection()
{
	int ret = mNetworkClient->Initialise("", mServerIP, 30004);

	if(ret == DREAMSOCK_CLIENT_ERROR)
	{
		char text[64];
		sprintf(text, "Could not open client socket");
	}
	Connect();
}

void GameClient::ReadPackets(void)
{
	char data[1400];
	struct sockaddr address;

	ClientSideClient *clList;

	int type;
	int ind;
	int local;
	int ret;
	int newTime;
	int time;

	char name[50];

	dreamMessage mes;
	mes.Init(data, sizeof(data));

	while(ret = mNetworkClient->GetPacket(mes.data, &address))
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

			AddClient(local, ind, name);
			break;

		case DREAMSOCK_MES_REMOVECLIENT:
			ind = mes.ReadByte();

			LogString("Got removeclient %d message", ind);

			RemoveClient(ind);

			break;

		case USER_MES_FRAME:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			newTime = dreamSock_GetCurrentSystemTime();
			time = newTime - mOldTime;
            mOldTime = newTime;

			for (int i = 0; i < mClientVector.size(); i++)
			{
				ReadDeltaMoveCommand(&mes, mClientVector.at(i));
				mClientVector.at(i)->mPlayer->processTick();
				MoveServerPlayer();
			}
			break;

		case USER_MES_NONDELTAFRAME:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			mOldTime = dreamSock_GetCurrentSystemTime();

			for (int i = 0; i < mClientVector.size(); i++)
			{
				LogString("Reading NONDELTAFRAME for client %d", mClientVector.at(i)->mIndex);
				ReadMoveCommand(&mes, mClientVector.at(i));
			}

			break;

		case USER_MES_SERVEREXIT:
			Disconnect();
			break;

		}
	}
}


void GameClient::RemoveClient(int ind)
{
}

void GameClient::SendCommand(void)
{
	if(mNetworkClient->GetConnectionState() != DREAMSOCK_CONNECTED)
		return;

	dreamMessage message;
	char data[1400];

	int i = mNetworkClient->GetOutgoingSequence() & (COMMAND_HISTORY_SIZE-1);

	message.Init(data, sizeof(data));
	message.WriteByte(USER_MES_FRAME);						// type
	message.AddSequences(mNetworkClient);					// sequences

	// Build delta-compressed move command
	BuildDeltaMoveCommand(&message, &mInputClient);

	// Send the packet
	mNetworkClient->SendPacket(&message);

	// Store the command to the input client's history
	memcpy(&mInputClient.mFrame[i], &mInputClient.mCommand, sizeof(Command));

	// Store the commands to the clients' history
	for (int i = 0; i < mClientVector.size(); i++)
	{
		memcpy(&mClientVector.at(i)->mFrame[i], &mClientVector.at(i)->mCommand, sizeof(Command));
	}
}

void GameClient::SendRequestNonDeltaFrame(void)
{
	char data[1400];
	dreamMessage message;
	message.Init(data, sizeof(data));

	message.WriteByte(USER_MES_NONDELTAFRAME);
	message.AddSequences(mNetworkClient);

	mNetworkClient->SendPacket(&message);
}

void GameClient::Connect(void)
{
	if(mInit)
	{
		LogString("ArmyWar already initialised");
		return;
	}

	LogString("GameClient::Connect");

	mInit = true;

	mNetworkClient->SendConnect("myname");
}

void GameClient::Disconnect(void)
{
	if(!mInit)
		return;

	LogString("GameClient::Disconnect");

	mInit = false;
	mLocalClient = NULL;
	memset(&mInputClient, 0, sizeof(ClientSideClient));

	mNetworkClient->SendDisconnect();
}

void GameClient::ReadMoveCommand(dreamMessage *mes, ClientSideClient *client)
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

void GameClient::ReadDeltaMoveCommand(dreamMessage *mes, ClientSideClient *client)
{
	int mProcessedFrame;
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

	if(flags & CMD_ORIGIN)
	{
		mProcessedFrame = mes->ReadByte();
	}

	// Origin
	if(flags & CMD_ORIGIN)
	{

	}

	// Read time to run command
	client->mServerFrame.mOrigin.x = mes->ReadFloat();
	client->mServerFrame.mOrigin.z = mes->ReadFloat();

	client->mServerFrame.mVelocity.x = mes->ReadFloat();
	client->mServerFrame.mVelocity.z = mes->ReadFloat();

	client->mCommand.mMilliseconds = mes->ReadByte();



	LogString("mVelocity.x:%f",client->mServerFrame.mVelocity.x);
	LogString("mVelocity.z:%f",client->mServerFrame.mVelocity.z);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc:
//-----------------------------------------------------------------------------
void GameClient::BuildDeltaMoveCommand(dreamMessage *mes, ClientSideClient *theClient)
{
	int flags = 0;
	int last = (mNetworkClient->GetOutgoingSequence() - 1) & (COMMAND_HISTORY_SIZE-1);

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

//-----------------------------------------------------------------------------
// Name: empty()
// Desc:
//-----------------------------------------------------------------------------
void GameClient::RunNetwork(int msec)
{
	static int time = 0;
	time += msec;

	ReadPackets();
	
	for (int i = 0; i < mClientVector.size(); i++)
	{
		mClientVector.at(i)->mPlayer->interpolateTick(mRendertime);
	}

	// Framerate is too high
	if(time > (1000 / 60)) {
		
		SendCommand();
		mFrametime = time / 1000.0f;
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
        game = new GameClient(strCmdLine);
#else
        game = new GameClient(argv[1]);
#endif

		//game = new GameClient;
	    game->StartConnection();

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		StartLogConsole();
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

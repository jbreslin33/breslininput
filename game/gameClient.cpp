#include "gameClient.h"
#include "../shape/clientSidePlayer.h"

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
    ClientSidePlayer* clientSidePlayer = new ClientSidePlayer(client,mSceneMgr,"jay" + index,0,0,0,"sinbad.mesh");
	clientSidePlayer->getSceneNode()->scale(30,30,30);
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
    ClientSidePlayer* clientSidePlayer = new ClientSidePlayer(client,mSceneMgr,"silentBob" + index,0,0,0,"sinbad.mesh");
	clientSidePlayer->getSceneNode()->scale(30,30,30);
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
	mInputClient.command.mKey = 0;
 	if(keys[VK_ESCAPE])
 	{
 		Shutdown();
 		keys[VK_ESCAPE] = false;
 	}
 	if(keys[VK_DOWN])
 	{
 		mInputClient.command.mKey |= KEY_DOWN;
 	}
 	if(keys[VK_UP])
 	{
 		mInputClient.command.mKey |= KEY_UP;
 	}
 	if(keys[VK_LEFT])
 	{
 		mInputClient.command.mKey |= KEY_LEFT;
 	}
 	if(keys[VK_RIGHT])
 	{
 		mInputClient.command.mKey |= KEY_RIGHT;
 	}
 	mInputClient.command.mMilliseconds = (int) (mFrametime * 1000);
 }

void GameClient::MoveServerPlayer(void)
{
    Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

	transVector.x = mLocalClient->serverFrame.mOrigin.x;
	transVector.z = -mLocalClient->serverFrame.mOrigin.z;

	if (mLocalClient->mServerPlayer)
	{
		//LogString("try to move server player");
			mLocalClient->mServerPlayer->getSceneNode()->setPosition(transVector);
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
				mClientVector.at(i)->mPlayer->ProcessTick();
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
	memcpy(&mInputClient.frame[i], &mInputClient.command, sizeof(ClientSideCommand));

	// Store the commands to the clients' history
	for (int i = 0; i < mClientVector.size(); i++)
	{
		memcpy(&mClientVector.at(i)->frame[i], &mClientVector.at(i)->command, sizeof(ClientSideCommand));
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
	client->serverFrame.mKey			= mes->ReadByte();

	// Origin
	client->serverFrame.mOrigin.x		= mes->ReadFloat();
	client->serverFrame.mOrigin.z		= mes->ReadFloat();
	client->serverFrame.mVelocity.x		= mes->ReadFloat();
	client->serverFrame.mVelocity.z		= mes->ReadFloat();

	// Read time to run command
	client->serverFrame.mMilliseconds = mes->ReadByte();

	memcpy(&client->command, &client->serverFrame, sizeof(ClientSideCommand));

	// Fill the history array with the position we got
	for(int f = 0; f < COMMAND_HISTORY_SIZE; f++)
	{
		client->frame[f].mPredictedOrigin.x = client->command.mOrigin.x;
		client->frame[f].mPredictedOrigin.z = client->command.mOrigin.z;
	}
}

void GameClient::ReadDeltaMoveCommand(dreamMessage *mes, ClientSideClient *client)
{
	int processedFrame;
	int flags = 0;

	// Flags
	flags = mes->ReadByte();

	// Key
	if(flags & CMD_KEY)
	{
		client->serverFrame.mKey = mes->ReadByte();

		client->command.mKey = client->serverFrame.mKey;
		LogString("Client %d: Read key %d", client->mIndex, client->command.mKey);
	}

	if(flags & CMD_ORIGIN)
	{
		processedFrame = mes->ReadByte();
	}

	// Origin
	if(flags & CMD_ORIGIN)
	{

	}

	// Read time to run command
	client->serverFrame.mOrigin.x = mes->ReadFloat();
	client->serverFrame.mOrigin.z = mes->ReadFloat();

	client->serverFrame.mVelocity.x = mes->ReadFloat();
	client->serverFrame.mVelocity.z = mes->ReadFloat();

	client->command.mMilliseconds = mes->ReadByte();
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
	if(theClient->frame[last].mKey != theClient->command.mKey)
		flags |= CMD_KEY;

	// Add to the message
	// Flags
	mes->WriteByte(flags);

	// Key
	if(flags & CMD_KEY)
	{
		mes->WriteByte(theClient->command.mKey);
	}

	mes->WriteByte(theClient->command.mMilliseconds);
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

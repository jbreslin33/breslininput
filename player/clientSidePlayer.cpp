#include "clientSidePlayer.h"


//key defines prob should be changed to a variable if possible
#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8

ClientSidePlayer::ClientSidePlayer(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr,
								   std::string mesh) : Player(name),
								   ClientSideMove(name,position,mSceneMgr,mesh),
								   ClientSideRotation(name,position,mSceneMgr,mesh)
{
	mIndex  = 0;
	mServerPlayer = NULL;
}

ClientSidePlayer::~ClientSidePlayer()
{
}



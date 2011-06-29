#ifndef OGRESHAPE_H
#define OGRESHAPE_H

//messages
#include "../../message/message.h"

//command
#include "../../command/command.h"

//game
#include "../../serverside/game/game.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

//utility
#include <string.h>

//math
#include "../../math/vector3D.h"

#define COMMAND_HISTORY_SIZE		64

class OgreShape 
{

public:
OgreShape();
OgreShape(Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::Root* root);
~OgreShape();

//ogre scene stuff
SceneNode*          mSceneNode;

//stats
Vector3D* mPosition;

int			mIndex;

Command mFrame[COMMAND_HISTORY_SIZE];
//Command	mServerFrame;					// the latest frame from server
Command	mCommand;						// current frame's commands
Command mLastCommand;     //this is for the aiguys

Message	mMessage; //is this all i need to get information over the internets?

//keys
Vector3 mKeyDirection;
Vector3 mGoalDirection;

Game* mGame;

};

#endif


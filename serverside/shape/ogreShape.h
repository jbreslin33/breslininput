#ifndef OGRESHAPE_H
#define OGRESHAPE_H

//messages
#include "../../message/message.h"

//command
#include "../../command/command.h"

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
OgreShape(std::string name, Vector3D* position, Ogre::Root* root);
~OgreShape();

//for tracking what we last sent to internets clients
	unsigned short	GetOutgoingSequence(void)				{ return outgoingSequence; }
	void			SetOutgoingSequence(unsigned short seq)	{ outgoingSequence = seq; }
	void			IncreaseOutgoingSequence(void)			{ outgoingSequence++; }
	unsigned short	outgoingSequence;		// Outgoing packet sequence

//ogre scene stuff
SceneNode*          mSceneNode;

//stats
Vector3D* mPosition;

int			mIndex;
std::string mName;

Command mFrame[COMMAND_HISTORY_SIZE];
Command	mServerFrame;					// the latest frame from server
Command	mCommand;						// current frame's commands

long mProcessedFrame;

Message	mMessage; //is this all i need to get information over the internets?

//keys
Vector3 mKeyDirection;
Vector3 mGoalDirection;

};

#endif


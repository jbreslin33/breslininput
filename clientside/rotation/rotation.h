#ifndef ROTATION_H
#define ROTATION_H

//parents
#include "../../clientside/shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class ClientSideRotationStateMachine;
class ClientSideShape;

class ClientSideRotation :  public virtual OgreShape
{

public:

ClientSideRotation(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~ClientSideRotation();

void setServerShape(ClientSideShape* serverShape) { mServerShape = serverShape; }

//movement
void processTick    ();

void interpolateTick    (float renderTime);

ClientSideShape* mServerShape;
ClientSideRotationStateMachine* mRotationStateMachine;

//vars
float mServerRotSpeed;
float mTurnSpeed;

float mRotInterpLimitHigh;
float mRotInterpLimitLow;
float mRotInterpIncrease; 
float mRotInterpDecrease;

//rotation
Ogre::Vector3 mServerRotOld;
Ogre::Vector3 mServerRotNew;
Ogre::Real    mDegreesToServer;
};

#endif


#ifndef CLIENTSIDEROTATION_H
#define CLIENTSIDEROTATION_H

//parents
#include "rotation.h"
#include "../shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class RotationStateMachine;
class ClientSideShape;

class ClientSideRotation :  public virtual OgreShape, public Rotation
{

public:

ClientSideRotation(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~ClientSideRotation();

void setServerShape(ClientSideShape* serverShape) { mServerShape = serverShape; }

//movement
void processTick    ();

void interpolateTick    (float renderTime);

ClientSideShape* mServerShape;

};

#endif


#ifndef CLIENTSIDEROTATION_H
#define CLIENTSIDEROTATION_H

//parents
#include "../shape/ogreShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class Command;
class RotationStateMachine;
class Shape;

class Rotation :  public virtual OgreShape
{

public:

Rotation(std::string name, Vector3D* position, Ogre::SceneManager* mSceneMgr, std::string mesh);
~Rotation();

void setServerShape(Shape* serverShape) { mServerShape = serverShape; }

//movement
void processTick    ();

void interpolateTick    (float renderTime);

Shape* mServerShape;
RotationStateMachine* mRotationStateMachine;

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


#ifndef CLIENTSIDEPLAYER_H
#define CLIENTSIDEPLAYER_H

#include "player.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

#define TURN_SPEED 500.0f      // character turning in degrees per second

class ClientSideClient;
class OgreShape;
class Command;
class MoveStateMachine;
class RotationStateMachine;

class ClientSidePlayer : public Player
{

public:

ClientSidePlayer(std::string name, ClientSideClient* client, OgreShape* shape);
~ClientSidePlayer();

//movement
void processTick    ();
void processRotation();

void interpolateTick    (float renderTime);
void interpolateRotation(float renderTime);


void calculateVelocity(Command *command, float frametime);

//associated client
ClientSideClient* mClient;

//associated Shape
OgreShape* mShape;

//move state machine
MoveStateMachine*   mMoveStateMachine;

//rotation state machine
RotationStateMachine* mRotationStateMachine;

float mServerRotSpeed;

float mRunSpeed;
float mTurnSpeed;
float mPosInterpLimitHigh;
float mPosInterpLimitLow;
float mPosInterpFactor;

float mRotInterpLimitHigh;
float mRotInterpLimitLow;
float mRotInterpIncrease; 
float mRotInterpDecrease;

//deltas
float mDeltaX;
float mDeltaZ;
float mDeltaPosition;

//rotation
Ogre::Vector3 mServerRotOld;
Ogre::Vector3 mServerRotNew;
Ogre::Real mDegreesToServer;

};

#endif


#ifndef SHAPE_H
#define SHAPE_H

//command
#include "../../command/command.h"

//utility
#include <string>

//math
#include "../../math/vector3D.h"

#define COMMAND_HISTORY_SIZE		64

class Game;
class DynamicShape;

class Shape
{
public:

Shape(Game* game,int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, std::string mesh);
~Shape();

virtual void     yaw                  (float amountToYaw, bool converToDegree   ) = 0;
virtual float    getDegreesToSomething(Vector3D something                       ) = 0;
virtual void     translate            (Vector3D translateVector, int perspective) = 0;
virtual Vector3D getPosition          (                                         ) = 0;
virtual void     setPosition          (Vector3D                                 ) = 0;
virtual void     setPosition          (float x, float y, float z                ) = 0;

//run speed
float mRunSpeed;

float mRunSpeedMax;

//id
int			mIndex;
std::string  mName;

Command	mServerFrame;					// the latest frame from server
Command mCommandToRunOnShape;

float mRenderTime;

Game* mGame;
DynamicShape* mGhost;

};

#endif


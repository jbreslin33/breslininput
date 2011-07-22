#ifndef OGRESHAPE_H
#define OGRESHAPE_H

#include "shape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class ObjectTitle;

class OgreShape : public Shape
{

public:
OgreShape(Game* game,int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::SceneManager* mSceneMgr, std::string mesh);
OgreShape();
~OgreShape();

SceneNode* getSceneNode() { return mSceneNode; }

void     yaw                  (float amountToYaw, bool converToDegree   );
float    getDegreesToSomething(Vector3D something                       );
void     translate            (Vector3D translateVector, int perspective);
Vector3D getPosition          (                                         );
void     setPosition          (Vector3D position                        );
void     setPosition          (float x, float y, float z                );

//ogre scene stuff
std::string         mMeshName;
Entity*             mEntity;
Ogre::SceneManager* mSceneManager;
SceneNode*          mSceneNode;

//billboard
ObjectTitle* mObjectTitle;
std::string  mObjectTitleString;

};

#endif


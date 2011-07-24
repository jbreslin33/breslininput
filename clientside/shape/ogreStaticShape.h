#ifndef OGRESTATICSHAPE_H
#define OGRESTATICSHAPE_H

#include "staticShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class ObjectTitle;

#include <string>

class OgreStaticShape : public StaticShape //, public OgreAnimation
{

public:
OgreStaticShape(Game* game, int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, std::string mesh);
~OgreStaticShape();

void checkExtents(Vector3D min);
void        scale                (Vector3D scaleVector);
Vector3D    getPosition          (                                         );
void        setPosition          (Vector3D position                        );
void        setPosition          (float x, float y, float z                );
void        setVisible           (bool visible                             );

std::string getName()      { return mName; } 
SceneNode*  getSceneNode() { return mSceneNode; }

Ogre::Vector3 converToVector3(Vector3D vector3d);

//ogre scene stuff
std::string         mMeshName;
std::string         mName;
Entity*             mEntity;
Ogre::SceneManager* mSceneManager;
SceneNode*          mSceneNode;

//billboard
ObjectTitle* mObjectTitle;
std::string  mObjectTitleString;

};

#endif


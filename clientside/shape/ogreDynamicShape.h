#ifndef OGREDYNAMICSHAPE_H
#define OGREDYNAMICSHAPE_H

#include "dynamicShape.h"

//Ogre headers
#include "Ogre.h"
using namespace Ogre;

class ObjectTitle;

#include <string>

class OgreDynamicShape : public DynamicShape //, public OgreAnimation
{

public:
OgreDynamicShape(Game* game, int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, std::string mesh);
~OgreDynamicShape();

//debugging
void        checkExtents         (Vector3D min);

//size
void        scale                (Vector3D scaleVector);

//movement
void        yaw                  (float amountToYaw, bool converToDegree   );
void        translate            (Vector3D translateVector, int perspective);
void        setPosition          (Vector3D position                        );
void        setPosition          (float x, float y, float z                );
void        setVisible           (bool visible                             );

//check position
float       getDegreesToSomething(Vector3D something                       );
Vector3D    getPosition          (                                         );

//title
void     drawTitle            (std::string title         );
void     appendToTile         (std::string appendage     );

//name
std::string getName()      { return mName; } 

//ogre scene node
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


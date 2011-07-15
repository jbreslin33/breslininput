#include "shape.h"
#include "../../tdreamsock/dreamSockLog.h"

#include "../../billboard/objectTitle.h"

#include <string.h>

Shape::Shape(int ind, Vector3D* position, Vector3D* velocity, Vector3D* rotation, Ogre::SceneManager* mSceneMgr,
								 std::string mesh)
:
	Move    (),
	Rotation(),
	OgreAnimation     (),
	OgreShape         (ind,position,velocity,rotation,mSceneMgr,mesh)
{

}

Shape::~Shape()
{

}

void Shape::processTick()
{

	//mObjectTitleString.clear(); //empty title string so it can be filled anew

	//call parent processTicks since you overrode them
	Move::processTick(); 
	Rotation::processTick();
	OgreAnimation::processTick();
	
	//run billboard here for now.
	//mObjectTitle->setTitle(mObjectTitleString); //fill title with mObjectTitleString which we have appended from all states we want info on.
	//mObjectTitle->update(); //draw title for shape

}
void Shape::interpolateTick(float renderTime)
{


	mRenderTime = renderTime;

	//call parent interpolateTicks since you overrode them
	Move::interpolateTick();
	Rotation::interpolateTick();
	OgreAnimation::interpolateTick();


}
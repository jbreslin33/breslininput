#include "serverSideRotationStates.h"
#include "serverSideRotationStateMachine.h"

#include "serverSideRotation.h"

Normal_Rotation* Normal_Rotation::Instance()
{
  static Normal_Rotation instance;
  return &instance;
}
void Normal_Rotation::enter(ServerSideRotation* rotation)
{
}
void Normal_Rotation::execute(ServerSideRotation* rotation)
{
	
}
void Normal_Rotation::exit(ServerSideRotation* rotation)
{
}

Catchup_Rotation* Catchup_Rotation::Instance()
{
  static Catchup_Rotation instance;
  return &instance;
}
void Catchup_Rotation::enter(ServerSideRotation* rotation)
{
}
void Catchup_Rotation::execute(ServerSideRotation* rotation)
{
	
}
void Catchup_Rotation::exit(ServerSideRotation* rotation)
{
}


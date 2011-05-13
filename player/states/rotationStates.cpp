#include "rotationStates.h"
#include "rotationStateMachine.h"

//player,client,shape combo
#include "../clientSidePlayer.h"
#include "../../client/clientSideClient.h"
#include "../../shape/ogreShape.h"

//only using for logstring this needs to be remedied.
#include "../../tdreamsock/dreamSock.h"

Normal_Rotation* Normal_Rotation::Instance()
{
  static Normal_Rotation instance;
  return &instance;
}
void Normal_Rotation::enter(ClientSidePlayer* player)
{
	LogString("STATE: Normal_Rotation");
}
void Normal_Rotation::execute(ClientSidePlayer* player)
{

}
void Normal_Rotation::exit(ClientSidePlayer* player)
{
}

Catchup_Rotation* Catchup_Rotation::Instance()
{
  static Catchup_Rotation instance;
  return &instance;
}
void Catchup_Rotation::enter(ClientSidePlayer* player)
{
	LogString("STATE: Catchup_Rotation");
}
void Catchup_Rotation::execute(ClientSidePlayer* player)
{

}
void Catchup_Rotation::exit(ClientSidePlayer* player)
{
}


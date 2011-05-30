#include "serverSideMoveStates.h"
#include "serverSideMoveStateMachine.h"

#include "serverSideMove.h"

Normal_Move* Normal_Move::Instance()
{
  static Normal_Move instance;
  return &instance;
}
void Normal_Move::enter(ServerSideMove* move)
{

}
void Normal_Move::execute(ServerSideMove* move)
{
	
}
void Normal_Move::exit(ServerSideMove* move)
{
}

Catchup_Move* Catchup_Move::Instance()
{
	static Catchup_Move instance;
	return &instance;
}
void Catchup_Move::enter(ServerSideMove* move)
{
}
void Catchup_Move::execute(ServerSideMove* move)
{
	
}
void Catchup_Move::exit(ServerSideMove* move)
{
}


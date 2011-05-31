#include "serverSideJumpStates.h"
#include "serverSideJumpStateMachine.h"

#include "serverSideJump.h"

Normal_Jump* Normal_Jump::Instance()
{
  static Normal_Jump instance;
  return &instance;
}
void Normal_Jump::enter(ServerSideJump* jump)
{

}
void Normal_Jump::execute(ServerSideJump* jump)
{
	
}
void Normal_Jump::exit(ServerSideJump* jump)
{
}

Catchup_Jump* Catchup_Jump::Instance()
{
	static Catchup_Jump instance;
	return &instance;
}
void Catchup_Jump::enter(ServerSideJump* jump)
{
}
void Catchup_Jump::execute(ServerSideJump* jump)
{
	
}
void Catchup_Jump::exit(ServerSideJump* jump)
{
}


#ifndef SERVERSIDEJUMPSTATES_H
#define SERVERSIDEJUMPSTATES_H
#include "serverSideJumpState.h"

class ServerSideJump;

class Normal_Jump : public ServerSideJumpState
{
private:
  Normal_Jump(){}
public:
  static Normal_Jump* Instance();
  void enter  (ServerSideJump* jump);
  void execute(ServerSideJump* jump);
  void exit   (ServerSideJump* jump);
};

class Catchup_Jump : public ServerSideJumpState
{
private:
  Catchup_Jump(){}
public:
  static Catchup_Jump* Instance();
  void enter  (ServerSideJump* jump);
  void execute(ServerSideJump* jump);
  void exit   (ServerSideJump* jump);
};

#endif

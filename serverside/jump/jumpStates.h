#ifndef SERVERSIDEJUMPSTATES_H
#define SERVERSIDEJUMPSTATES_H
#include "jumpState.h"

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

class No_Jump : public ServerSideJumpState
{
private:
  No_Jump(){}
public:
  static No_Jump* Instance();
  void enter  (ServerSideJump* jump);
  void execute(ServerSideJump* jump);
  void exit   (ServerSideJump* jump);
};

#endif

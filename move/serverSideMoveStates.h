#ifndef SERVERSIDEMOVESTATES_H
#define SERVERSIDEMOVESTATES_H
#include "serverSideMoveState.h"

class ServerSideMove;

class Normal_Move : public ServerSideMoveState
{
private:
  Normal_Move(){}
public:
  static Normal_Move* Instance();
  void enter  (ServerSideMove* move);
  void execute(ServerSideMove* move);
  void exit   (ServerSideMove* move);
};

class Catchup_Move : public ServerSideMoveState
{
private:
  Catchup_Move(){}
public:
  static Catchup_Move* Instance();
  void enter  (ServerSideMove* move);
  void execute(ServerSideMove* move);
  void exit   (ServerSideMove* move);
};

#endif

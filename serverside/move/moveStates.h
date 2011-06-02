#ifndef MOVESTATES_H
#define MOVESTATES_H
#include "moveState.h"

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

class No_Move : public ServerSideMoveState
{
private:
  No_Move(){}
public:
  static No_Move* Instance();
  void enter  (ServerSideMove* move);
  void execute(ServerSideMove* move);
  void exit   (ServerSideMove* move);
};

class Accelerate_Move : public ServerSideMoveState
{
private:
  Accelerate_Move(){}
public:
  static Accelerate_Move* Instance();
  void enter  (ServerSideMove* move);
  void execute(ServerSideMove* move);
  void exit   (ServerSideMove* move);
};

class Decelerate_Move : public ServerSideMoveState
{
private:
  Decelerate_Move(){}
public:
  static Decelerate_Move* Instance();
  void enter  (ServerSideMove* move);
  void execute(ServerSideMove* move);
  void exit   (ServerSideMove* move);
};
#endif

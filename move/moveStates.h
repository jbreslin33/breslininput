#ifndef MOVESTATES_H
#define MOVESTATES_H
#include "moveState.h"

class ClientSideMove;

class Normal_Move : public MoveState
{
private:
  Normal_Move(){}
public:
  static Normal_Move* Instance();
  void enter  (ClientSideMove* move);
  void execute(ClientSideMove* move);
  void exit   (ClientSideMove* move);
};

class Catchup_Move : public MoveState
{
private:
  Catchup_Move(){}
public:
  static Catchup_Move* Instance();
  void enter  (ClientSideMove* move);
  void execute(ClientSideMove* move);
  void exit   (ClientSideMove* move);
};

#endif

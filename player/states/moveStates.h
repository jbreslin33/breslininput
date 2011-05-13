#ifndef MOVESTATES_H
#define MOVESTATES_H
#include "moveState.h"

class ClientSidePlayer;

class Normal_Move : public MoveState
{
private:
  Normal_Move(){}
public:
  static Normal_Move* Instance();
  void enter  (ClientSidePlayer* player);
  void execute(ClientSidePlayer* player);
  void exit   (ClientSidePlayer* player);
};

class Catchup_Move : public MoveState
{
private:
  Catchup_Move(){}
public:
  static Catchup_Move* Instance();
  void enter  (ClientSidePlayer* player);
  void execute(ClientSidePlayer* player);
  void exit   (ClientSidePlayer* player);
};

#endif
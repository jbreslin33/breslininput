#ifndef CLIENTSIDEMOVESTATES_H
#define CLIENTSIDEMOVESTATES_H
#include "clientSideMoveState.h"

class ClientSideMove;

class Normal_Move : public ClientSideMoveState
{
private:
  Normal_Move(){}
public:
  static Normal_Move* Instance();
  void enter  (ClientSideMove* move);
  void execute(ClientSideMove* move);
  void exit   (ClientSideMove* move);
};

class Catchup_Move : public ClientSideMoveState
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

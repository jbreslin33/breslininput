#ifndef MOVESTATES_H
#define MOVESTATES_H
#include "moveState.h"

class Move;

class Normal_Move : public MoveState
{
private:
  Normal_Move(){}
public:
  static Normal_Move* Instance();
  void enter  (Move* move);
  void execute(Move* move);
  void exit   (Move* move);
};

class Catchup_Move : public MoveState
{
private:
  Catchup_Move(){}
public:
  static Catchup_Move* Instance();
  void enter  (Move* move);
  void execute(Move* move);
  void exit   (Move* move);
};

#endif

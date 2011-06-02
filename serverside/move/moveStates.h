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

class No_Move : public MoveState
{
private:
  No_Move(){}
public:
  static No_Move* Instance();
  void enter  (Move* move);
  void execute(Move* move);
  void exit   (Move* move);
};

class Accelerate_Move : public MoveState
{
private:
  Accelerate_Move(){}
public:
  static Accelerate_Move* Instance();
  void enter  (Move* move);
  void execute(Move* move);
  void exit   (Move* move);
};

class Decelerate_Move : public MoveState
{
private:
  Decelerate_Move(){}
public:
  static Decelerate_Move* Instance();
  void enter  (Move* move);
  void execute(Move* move);
  void exit   (Move* move);
};
#endif

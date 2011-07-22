#ifndef MOVESTATES_H
#define MOVESTATES_H
#include "moveState.h"

class Move;

class Global_ProcessTick_Move : public MoveState
{
private:
  Global_ProcessTick_Move(){}
public:
  static Global_ProcessTick_Move* Instance();
  void enter  (Move* move);
  void execute(Move* move);
  void exit   (Move* move);
};

class Normal_ProcessTick_Move : public MoveState
{
private:
  Normal_ProcessTick_Move(){}
public:
  static Normal_ProcessTick_Move* Instance();
  void enter  (Move* move);
  void execute(Move* move);
  void exit   (Move* move);
};

class Catchup_ProcessTick_Move : public MoveState
{
private:
  Catchup_ProcessTick_Move(){}
public:
  static Catchup_ProcessTick_Move* Instance();
  void enter  (Move* move);
  void execute(Move* move);
  void exit   (Move* move);
};


class Global_InterpolateTick_Move : public MoveState
{
private:
  Global_InterpolateTick_Move(){}
public:
  static Global_InterpolateTick_Move* Instance();
  void enter  (Move* move);
  void execute(Move* move);
  void exit   (Move* move);
};

class Normal_InterpolateTick_Move : public MoveState
{
private:
  Normal_InterpolateTick_Move(){}
public:
  static Normal_InterpolateTick_Move* Instance();
  void enter  (Move* move);
  void execute(Move* move);
  void exit   (Move* move);
};

class Off_InterpolateTick_Move : public MoveState
{
private:
  Off_InterpolateTick_Move(){}
public:
  static Off_InterpolateTick_Move* Instance();
  void enter  (Move* move);
  void execute(Move* move);
  void exit   (Move* move);
};


#endif

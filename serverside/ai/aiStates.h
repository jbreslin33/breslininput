#ifndef AISTATES_H
#define AISTATES_H
#include "aiState.h"

class AI;

class Normal_AI : public AIState
{
private:
  Normal_AI(){}
public:
  static Normal_AI* Instance();
  void enter  (AI* ai);
  void execute(AI* ai);
  void exit   (AI* ai);
};

class Random_AI : public AIState
{
private:
  Random_AI(){}
public:
  static Random_AI* Instance();
  void enter  (AI* ai);
  void execute(AI* ai);
  void exit   (AI* ai);
};

class No_AI : public AIState
{
private:
  No_AI(){}
public:
  static No_AI* Instance();
  void enter  (AI* ai);
  void execute(AI* ai);
  void exit   (AI* ai);
};

class Accelerate_AI : public AIState
{
private:
  Accelerate_AI(){}
public:
  static Accelerate_AI* Instance();
  void enter  (AI* ai);
  void execute(AI* ai);
  void exit   (AI* ai);
};

class Decelerate_AI : public AIState
{
private:
  Decelerate_AI(){}
public:
  static Decelerate_AI* Instance();
  void enter  (AI* ai);
  void execute(AI* ai);
  void exit   (AI* ai);
};
#endif

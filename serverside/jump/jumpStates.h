#ifndef SERVERSIDEJUMPSTATES_H
#define SERVERSIDEJUMPSTATES_H
#include "jumpState.h"

class Jump;

class Normal_Jump : public JumpState
{
private:
  Normal_Jump(){}
public:
  static Normal_Jump* Instance();
  void enter  (Jump* jump);
  void execute(Jump* jump);
  void exit   (Jump* jump);
};

class No_Jump : public JumpState
{
private:
  No_Jump(){}
public:
  static No_Jump* Instance();
  void enter  (Jump* jump);
  void execute(Jump* jump);
  void exit   (Jump* jump);
};

#endif

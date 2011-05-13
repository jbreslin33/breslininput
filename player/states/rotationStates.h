#ifndef ROTATIONSTATES_H
#define ROTATIONSTATES_H
#include "rotationState.h"

class ClientSidePlayer;

class Normal_Rotation : public RotationState
{
private:
  Normal_Rotation(){}
public:
  static Normal_Rotation* Instance();
  void enter  (ClientSidePlayer* player);
  void execute(ClientSidePlayer* player);
  void exit   (ClientSidePlayer* player);
};

class Catchup_Rotation : public RotationState
{
private:
  Catchup_Rotation(){}
public:
  static Catchup_Rotation* Instance();
  void enter  (ClientSidePlayer* player);
  void execute(ClientSidePlayer* player);
  void exit   (ClientSidePlayer* player);
};

#endif
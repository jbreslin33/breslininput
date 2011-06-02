#ifndef ROTATIONSTATES_H
#define ROTATIONSTATES_H
#include "rotationState.h"

class ServerSideRotation;

class Normal_Rotation : public ServerSideRotationState
{
private:
  Normal_Rotation(){}
public:
  static Normal_Rotation* Instance();
  void enter  (ServerSideRotation* rotation);
  void execute(ServerSideRotation* rotation);
  void exit   (ServerSideRotation* rotation);
};

class No_Rotation : public ServerSideRotationState
{
private:
  No_Rotation(){}
public:
  static No_Rotation* Instance();
  void enter  (ServerSideRotation* rotation);
  void execute(ServerSideRotation* rotation);
  void exit   (ServerSideRotation* rotation);
};

#endif
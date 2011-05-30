#ifndef CLIENTSIDEROTATIONSTATES_H
#define CLIENTSIDEROTATIONSTATES_H
#include "clientSideRotationState.h"

class ClientSideRotation;

class Normal_Rotation : public ClientSideRotationState
{
private:
  Normal_Rotation(){}
public:
  static Normal_Rotation* Instance();
  void enter  (ClientSideRotation* rotation);
  void execute(ClientSideRotation* rotation);
  void exit   (ClientSideRotation* rotation);
};

class Catchup_Rotation : public ClientSideRotationState
{
private:
  Catchup_Rotation(){}
public:
  static Catchup_Rotation* Instance();
  void enter  (ClientSideRotation* rotation);
  void execute(ClientSideRotation* rotation);
  void exit   (ClientSideRotation* rotation);
};

#endif
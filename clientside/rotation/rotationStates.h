#ifndef ROTATIONSTATES_H
#define ROTATIONSTATES_H
#include "rotationState.h"

class Rotation;

class Global_ProcessTick_Rotation : public RotationState
{
private:
  Global_ProcessTick_Rotation(){}
public:
  static Global_ProcessTick_Rotation* Instance();
  void enter  (Rotation* rotation);
  void execute(Rotation* rotation);
  void exit   (Rotation* rotation);
};

class Normal_ProcessTick_Rotation : public RotationState
{
private:
  Normal_ProcessTick_Rotation(){}
public:
  static Normal_ProcessTick_Rotation* Instance();
  void enter  (Rotation* rotation);
  void execute(Rotation* rotation);
  void exit   (Rotation* rotation);
};

class Catchup_ProcessTick_Rotation : public RotationState
{
private:
  Catchup_ProcessTick_Rotation(){}
public:
  static Catchup_ProcessTick_Rotation* Instance();
  void enter  (Rotation* rotation);
  void execute(Rotation* rotation);
  void exit   (Rotation* rotation);
};
/********************InterpolateTick*************/

class Global_InterpolateTick_Rotation : public RotationState
{
private:
  Global_InterpolateTick_Rotation(){}
public:
  static Global_InterpolateTick_Rotation* Instance();
  void enter  (Rotation* rotation);
  void execute(Rotation* rotation);
  void exit   (Rotation* rotation);
};

class Normal_InterpolateTick_Rotation : public RotationState
{
private:
  Normal_InterpolateTick_Rotation(){}
public:
  static Normal_InterpolateTick_Rotation* Instance();
  void enter  (Rotation* rotation);
  void execute(Rotation* rotation);
  void exit   (Rotation* rotation);
};

class Off_InterpolateTick_Rotation : public RotationState
{
private:
  Off_InterpolateTick_Rotation(){}
public:
  static Off_InterpolateTick_Rotation* Instance();
  void enter  (Rotation* rotation);
  void execute(Rotation* rotation);
  void exit   (Rotation* rotation);
};
#endif
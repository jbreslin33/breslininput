#ifndef DYNAMICSHAPEROTATIONSTATES_H
#define DYNAMICSHAPEROTATIONSTATES_H
#include "dynamicShapeState.h"

class DynamicShape;

class Global_ProcessTick_Rotation : public DynamicShapeState
{
private:
  Global_ProcessTick_Rotation(){}
public:
  static Global_ProcessTick_Rotation* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};

class Normal_ProcessTick_Rotation : public DynamicShapeState
{
private:
  Normal_ProcessTick_Rotation(){}
public:
  static Normal_ProcessTick_Rotation* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};

class Catchup_ProcessTick_Rotation : public DynamicShapeState
{
private:
  Catchup_ProcessTick_Rotation(){}
public:
  static Catchup_ProcessTick_Rotation* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};
/********************InterpolateTick*************/

class Global_InterpolateTick_Rotation : public DynamicShapeState
{
private:
  Global_InterpolateTick_Rotation(){}
public:
  static Global_InterpolateTick_Rotation* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};

class Normal_InterpolateTick_Rotation : public DynamicShapeState
{
private:
  Normal_InterpolateTick_Rotation(){}
public:
  static Normal_InterpolateTick_Rotation* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};

class Off_InterpolateTick_Rotation : public DynamicShapeState
{
private:
  Off_InterpolateTick_Rotation(){}
public:
  static Off_InterpolateTick_Rotation* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};
#endif
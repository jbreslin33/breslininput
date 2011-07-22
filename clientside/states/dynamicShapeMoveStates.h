#ifndef DYNAMICSHAPEMOVESTATES_H
#define DYNAMICSHAPEMOVESTATES_H
#include "dynamicShapeState.h"

class DynamicShape;

class Global_ProcessTick_Move : public DynamicShapeState
{
private:
  Global_ProcessTick_Move(){}
public:
  static Global_ProcessTick_Move* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};

class Normal_ProcessTick_Move : public DynamicShapeState
{
private:
  Normal_ProcessTick_Move(){}
public:
  static Normal_ProcessTick_Move* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};

class Catchup_ProcessTick_Move : public DynamicShapeState
{
private:
  Catchup_ProcessTick_Move(){}
public:
  static Catchup_ProcessTick_Move* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};


class Global_InterpolateTick_Move : public DynamicShapeState
{
private:
  Global_InterpolateTick_Move(){}
public:
  static Global_InterpolateTick_Move* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};

class Normal_InterpolateTick_Move : public DynamicShapeState
{
private:
  Normal_InterpolateTick_Move(){}
public:
  static Normal_InterpolateTick_Move* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};

class Off_InterpolateTick_Move : public DynamicShapeState
{
private:
  Off_InterpolateTick_Move(){}
public:
  static Off_InterpolateTick_Move* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};


#endif

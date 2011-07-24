#ifndef DYNAMICSHAPEANIMATIONSTATES_H
#define DYNAMICSHAPEANIMATIONSTATES_H
#include "dynamicShapeState.h"

class DynamicShape;

/********************InterpolateTick*************/

class Global_InterpolateTick_Animation : public DynamicShapeState
{
private:
  Global_InterpolateTick_Animation(){}
public:
  static Global_InterpolateTick_Animation* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};

class Idle_InterpolateTick_Animation : public DynamicShapeState
{
private:
  Idle_InterpolateTick_Animation(){}
public:
  static Idle_InterpolateTick_Animation* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};

class Run_InterpolateTick_Animation : public DynamicShapeState
{
private:
  Run_InterpolateTick_Animation(){}
public:
  static Run_InterpolateTick_Animation* Instance();
  void enter  (DynamicShape* dynamicShape);
  void execute(DynamicShape* dynamicShape);
  void exit   (DynamicShape* dynamicShape);
};

#endif
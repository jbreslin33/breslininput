#ifndef ANIMATIONSTATES_H
#define ANIMATIONSTATES_H
#include "animationState.h"

class OgreAnimation;

class Global_ProcessTick_Animation : public AnimationState
{
private:
  Global_ProcessTick_Animation(){}
public:
  static Global_ProcessTick_Animation* Instance();
  void enter  (OgreAnimation* animation);
  void execute(OgreAnimation* animation);
  void exit   (OgreAnimation* animation);
};

class Normal_ProcessTick_Animation : public AnimationState
{
private:
  Normal_ProcessTick_Animation(){}
public:
  static Normal_ProcessTick_Animation* Instance();
  void enter  (OgreAnimation* animation);
  void execute(OgreAnimation* animation);
  void exit   (OgreAnimation* animation);
};

class Catchup_ProcessTick_Animation : public AnimationState
{
private:
  Catchup_ProcessTick_Animation(){}
public:
  static Catchup_ProcessTick_Animation* Instance();
  void enter  (OgreAnimation* animation);
  void execute(OgreAnimation* animation);
  void exit   (OgreAnimation* animation);
};
/********************InterpolateTick*************/

class Global_InterpolateTick_Animation : public AnimationState
{
private:
  Global_InterpolateTick_Animation(){}
public:
  static Global_InterpolateTick_Animation* Instance();
  void enter  (OgreAnimation* animation);
  void execute(OgreAnimation* animation);
  void exit   (OgreAnimation* animation);
};

class Normal_InterpolateTick_Animation : public AnimationState
{
private:
  Normal_InterpolateTick_Animation(){}
public:
  static Normal_InterpolateTick_Animation* Instance();
  void enter  (OgreAnimation* animation);
  void execute(OgreAnimation* animation);
  void exit   (OgreAnimation* animation);
};

class Off_InterpolateTick_Animation : public AnimationState
{
private:
  Off_InterpolateTick_Animation(){}
public:
  static Off_InterpolateTick_Animation* Instance();
  void enter  (OgreAnimation* animation);
  void execute(OgreAnimation* animation);
  void exit   (OgreAnimation* animation);
};
#endif
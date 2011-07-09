#ifndef ANIMATIONSTATES_H
#define ANIMATIONSTATES_H
#include "animationState.h"

class OgreAnimation;

class Global_ProcessTick_Animation : public OgreAnimationState
{
private:
  Global_ProcessTick_Animation(){}
public:
  static Global_ProcessTick_Animation* Instance();
  void enter  (OgreAnimation* animation);
  void execute(OgreAnimation* animation);
  void exit   (OgreAnimation* animation);
};

class Idle_ProcessTick_Animation : public OgreAnimationState
{
private:
  Idle_ProcessTick_Animation(){}
public:
  static Idle_ProcessTick_Animation* Instance();
  void enter  (OgreAnimation* animation);
  void execute(OgreAnimation* animation);
  void exit   (OgreAnimation* animation);
};

class Run_ProcessTick_Animation : public OgreAnimationState
{
private:
  Run_ProcessTick_Animation(){}
public:
  static Run_ProcessTick_Animation* Instance();
  void enter  (OgreAnimation* animation);
  void execute(OgreAnimation* animation);
  void exit   (OgreAnimation* animation);
};
/********************InterpolateTick*************/

class Global_InterpolateTick_Animation : public OgreAnimationState
{
private:
  Global_InterpolateTick_Animation(){}
public:
  static Global_InterpolateTick_Animation* Instance();
  void enter  (OgreAnimation* animation);
  void execute(OgreAnimation* animation);
  void exit   (OgreAnimation* animation);
};

class Idle_InterpolateTick_Animation : public OgreAnimationState
{
private:
  Idle_InterpolateTick_Animation(){}
public:
  static Idle_InterpolateTick_Animation* Instance();
  void enter  (OgreAnimation* animation);
  void execute(OgreAnimation* animation);
  void exit   (OgreAnimation* animation);
};

class Run_InterpolateTick_Animation : public OgreAnimationState
{
private:
  Run_InterpolateTick_Animation(){}
public:
  static Run_InterpolateTick_Animation* Instance();
  void enter  (OgreAnimation* animation);
  void execute(OgreAnimation* animation);
  void exit   (OgreAnimation* animation);
};

class Off_InterpolateTick_Animation : public OgreAnimationState
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
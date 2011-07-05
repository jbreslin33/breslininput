#ifndef ANIMATIONSTATE_H
#define ANIMATIONSTATE_H

class Animation;

class AnimationState
{
public:

  virtual ~AnimationState(){}

  //this will execute when the state is entered
  virtual void enter(Animation*)=0;

  //this is the states normal update function
  virtual void execute(Animation*)=0;

  //this will execute when the state is exited. (My word, isn't
  //life full of surprises... ;o))
  virtual void exit(Animation*)=0;
};

#endif
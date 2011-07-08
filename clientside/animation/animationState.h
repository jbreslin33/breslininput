#ifndef ANIMATIONSTATE_H
#define ANIMATIONSTATE_H

class OgreAnimation;

class AnimationState
{
public:

  virtual ~AnimationState(){}

  //this will execute when the state is entered
  virtual void enter(OgreAnimation*)=0;

  //this is the states normal update function
  virtual void execute(OgreAnimation*)=0;

  //this will execute when the state is exited. (My word, isn't
  //life full of surprises... ;o))
  virtual void exit(OgreAnimation*)=0;
};

#endif
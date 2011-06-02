#ifndef SERVERSIDEROTATIONSTATE_H
#define SERVERSIDEROTATIONSTATE_H

class Rotation;

class RotationState
{
public:

  virtual ~RotationState(){}

  //this will execute when the state is entered
  virtual void enter(Rotation*)=0;

  //this is the states normal update function
  virtual void execute(Rotation*)=0;

  //this will execute when the state is exited. (My word, isn't
  //life full of surprises... ;o))
  virtual void exit(Rotation*)=0;
};

#endif
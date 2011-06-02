#ifndef ClIENTSIDEMOVESTATE_H
#define ClIENTSIDEMOVESTATE_H

class Jump;

class JumpState
{
public:

  virtual ~JumpState(){}

  //this will execute when the state is entered
  virtual void enter(Jump*)=0;

  //this is the states normal update function
  virtual void execute(Jump*)=0;

  //this will execute when the state is exited. (My word, isn't
  //life full of surprises... ;o))
  virtual void exit(Jump*)=0;
};

#endif

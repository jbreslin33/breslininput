#ifndef ClIENTSIDEMOVESTATE_H
#define ClIENTSIDEMOVESTATE_H

class Move;

class MoveState
{
public:

  virtual ~MoveState(){}

  //this will execute when the state is entered
  virtual void enter(Move*)=0;

  //this is the states normal update function
  virtual void execute(Move*)=0;

  //this will execute when the state is exited. (My word, isn't
  //life full of surprises... ;o))
  virtual void exit(Move*)=0;
};

#endif

#ifndef ClIENTSIDEAISTATE_H
#define ClIENTSIDEAISTATE_H

class AI;

class AIState
{
public:

  virtual ~AIState(){}

  //this will execute when the state is entered
  virtual void enter(AI*)=0;

  //this is the states normal update function
  virtual void execute(AI*)=0;

  //this will execute when the state is exited. (My word, isn't
  //life full of surprises... ;o))
  virtual void exit(AI*)=0;
};

#endif

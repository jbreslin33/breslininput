#ifndef ClIENTSIDEMOVESTATE_H
#define ClIENTSIDEMOVESTATE_H

class ClientSideMove;

class ClientSideMoveState
{
public:

  virtual ~ClientSideMoveState(){}

  //this will execute when the state is entered
  virtual void enter(ClientSideMove*)=0;

  //this is the states normal update function
  virtual void execute(ClientSideMove*)=0;

  //this will execute when the state is exited. (My word, isn't
  //life full of surprises... ;o))
  virtual void exit(ClientSideMove*)=0;
};

#endif

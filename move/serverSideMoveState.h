#ifndef ClIENTSIDEMOVESTATE_H
#define ClIENTSIDEMOVESTATE_H

class ServerSideMove;

class ServerSideMoveState
{
public:

  virtual ~ServerSideMoveState(){}

  //this will execute when the state is entered
  virtual void enter(ServerSideMove*)=0;

  //this is the states normal update function
  virtual void execute(ServerSideMove*)=0;

  //this will execute when the state is exited. (My word, isn't
  //life full of surprises... ;o))
  virtual void exit(ServerSideMove*)=0;
};

#endif

#ifndef ClIENTSIDEMOVESTATE_H
#define ClIENTSIDEMOVESTATE_H

class ServerSideJump;

class ServerSideJumpState
{
public:

  virtual ~ServerSideJumpState(){}

  //this will execute when the state is entered
  virtual void enter(ServerSideJump*)=0;

  //this is the states normal update function
  virtual void execute(ServerSideJump*)=0;

  //this will execute when the state is exited. (My word, isn't
  //life full of surprises... ;o))
  virtual void exit(ServerSideJump*)=0;
};

#endif

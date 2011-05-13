#ifndef MOVESTATE_H
#define MOVESTATE_H

class ClientSidePlayer;

class MoveState
{
public:

  virtual ~MoveState(){}

  //this will execute when the state is entered
  virtual void enter(ClientSidePlayer*)=0;

  //this is the states normal update function
  virtual void execute(ClientSidePlayer*)=0;

  //this will execute when the state is exited. (My word, isn't
  //life full of surprises... ;o))
  virtual void exit(ClientSidePlayer*)=0;
};

#endif
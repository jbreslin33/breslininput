#ifndef CLIENTSIDEROTATIONSTATE_H
#define CLIENTSIDEROTATIONSTATE_H

class ClientSideRotation;

class ClientSideRotationState
{
public:

  virtual ~ClientSideRotationState(){}

  //this will execute when the state is entered
  virtual void enter(ClientSideRotation*)=0;

  //this is the states normal update function
  virtual void execute(ClientSideRotation*)=0;

  //this will execute when the state is exited. (My word, isn't
  //life full of surprises... ;o))
  virtual void exit(ClientSideRotation*)=0;
};

#endif
#ifndef SERVERSIDEROTATIONSTATE_H
#define SERVERSIDEROTATIONSTATE_H

class ServerSideRotation;

class ServerSideRotationState
{
public:

  virtual ~ServerSideRotationState(){}

  //this will execute when the state is entered
  virtual void enter(ServerSideRotation*)=0;

  //this is the states normal update function
  virtual void execute(ServerSideRotation*)=0;

  //this will execute when the state is exited. (My word, isn't
  //life full of surprises... ;o))
  virtual void exit(ServerSideRotation*)=0;
};

#endif
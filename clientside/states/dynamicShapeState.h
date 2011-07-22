#ifndef DYNAMICSHAPESTATE_H
#define DYNAMICSHAPESTATE_H

class DynamicShape;

class DynamicShapeState
{
public:

  virtual ~DynamicShapeState(){}

  //this will execute when the state is entered
  virtual void enter(DynamicShape*)=0;

  //this is the states normal update function
  virtual void execute(DynamicShape*)=0;

  //this will execute when the state is exited. (My word, isn't
  //life full of surprises... ;o))
  virtual void exit(DynamicShape*)=0;
};

#endif

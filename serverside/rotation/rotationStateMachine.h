#ifndef ROTATIONSTATEMACHINE_H
#define ROTATIONSTATEMACHINE_H
#include "rotationState.h"

class Rotation;

class RotationStateMachine
{
private:
  //a pointer to the agent that owns this instance
  Rotation*   m_pOwner;
  RotationState*    m_pCurrentState;

  //a record of the last state the agent was in
  RotationState*   m_pPreviousState;

  //this is called every time the FSM is updated
  RotationState*   m_pGlobalState;

public:

  RotationStateMachine(Rotation* owner):m_pOwner(owner),
	                               m_pCurrentState(0),
                                   m_pPreviousState(0),
                                   m_pGlobalState(0)
  {}

  virtual ~RotationStateMachine(){}

  //use these methods to initialize the FSM
  void setCurrentState(RotationState* s){m_pCurrentState = s;}
  void setGlobalState(RotationState* s) {m_pGlobalState = s;}
  void setPreviousState(RotationState* s){m_pPreviousState = s;}

  //call this to update the FSM
  void  update()const
  {
    //if a global state exists, call its execute method, else do nothing
    //if(m_pGlobalState)   m_pGlobalState->Execute(m_pOwner);
    //same for the current state
	    //Con::errorf("fieldPlayerStateMachine update");
    if (m_pCurrentState) m_pCurrentState->execute(m_pOwner);
  }

  //change to a new state
  void  changeState(RotationState* pNewState)
  {
   // assert(pNewState &&
           //"<StateMachine::ChangeState>: trying to change to NULL state");

    //keep a record of the previous state
    m_pPreviousState = m_pCurrentState;

    //call the exit method of the existing state
	if(m_pCurrentState)
       m_pCurrentState->exit(m_pOwner);

    //change state to the new state
    m_pCurrentState = pNewState;

    //call the entry method of the new state
	if(m_pCurrentState)
       m_pCurrentState->enter(m_pOwner);
  }

  //change state back to the previous state
  void  revertToPreviousState()
  {
    changeState(m_pPreviousState);
  }

  //returns true if the current state's type is equal to the type of the
  //class passed as a parameter.
 // bool  isInState(const State& st)const
 // {
 //   return typeid(*m_pCurrentState) == typeid(st);
 // }

  RotationState*  currentState()  const{return m_pCurrentState;}
  RotationState*  globalState()   const{return m_pGlobalState;}
  RotationState*  previousState() const{return m_pPreviousState;}
};
#endif



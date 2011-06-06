#ifndef MOVESTATEMACHINE_H
#define MOVESTATEMACHINE_H
#include "moveState.h"

class Move;

class MoveStateMachine
{
private:
  //a pointer to the agent that owns this instance
  Move*   m_pOwner;
  MoveState*    m_pCurrentState;

  //a record of the last state the agent was in
  MoveState*   m_pPreviousState;

  //this is called every time the FSM is updated
  MoveState*   m_pGlobalState;

public:

  MoveStateMachine(Move* owner):m_pOwner(owner),
	                               m_pCurrentState(0),
                                   m_pPreviousState(0),
                                   m_pGlobalState(0)
  {}

  virtual ~MoveStateMachine(){}

  //use these methods to initialize the FSM
  void setCurrentState(MoveState* s){m_pCurrentState = s;}
  void setGlobalState(MoveState* s) {m_pGlobalState = s;}
  void setPreviousState(MoveState* s){m_pPreviousState = s;}

  //call this to update the FSM
  void  update()const
  {
    //if a global state exists, call its execute method, else do nothing
    if(m_pGlobalState)   m_pGlobalState->execute(m_pOwner);
    //same for the current state
	    //Con::errorf("fieldPlayerStateMachine update");
    if (m_pCurrentState) m_pCurrentState->execute(m_pOwner);
  }

  //change to a new state
  void  changeState(MoveState* pNewState)
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

  MoveState*  currentState()  const{return m_pCurrentState;}
  MoveState*  globalState()   const{return m_pGlobalState;}
  MoveState*  previousState() const{return m_pPreviousState;}
};
#endif



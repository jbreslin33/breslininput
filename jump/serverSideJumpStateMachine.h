#ifndef SERVERSIDEJUMPSTATEMACHINE_H
#define SERVERSIDEJUMPSTATEMACHINE_H
#include "serverSideJumpState.h"

class ServerSideJump;

class ServerSideJumpStateMachine
{
private:
  //a pointer to the agent that owns this instance
  ServerSideJump*   m_pOwner;
  ServerSideJumpState*    m_pCurrentState;

  //a record of the last state the agent was in
  ServerSideJumpState*   m_pPreviousState;

  //this is called every time the FSM is updated
  ServerSideJumpState*   m_pGlobalState;

public:

  ServerSideJumpStateMachine(ServerSideJump* owner):m_pOwner(owner),
	                               m_pCurrentState(0),
                                   m_pPreviousState(0),
                                   m_pGlobalState(0)
  {}

  virtual ~ServerSideJumpStateMachine(){}

  //use these methods to initialize the FSM
  void setCurrentState(ServerSideJumpState* s){m_pCurrentState = s;}
  void setGlobalState(ServerSideJumpState* s) {m_pGlobalState = s;}
  void setPreviousState(ServerSideJumpState* s){m_pPreviousState = s;}

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
  void  changeState(ServerSideJumpState* pNewState)
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

  ServerSideJumpState*  currentState()  const{return m_pCurrentState;}
  ServerSideJumpState*  globalState()   const{return m_pGlobalState;}
  ServerSideJumpState*  previousState() const{return m_pPreviousState;}
};
#endif



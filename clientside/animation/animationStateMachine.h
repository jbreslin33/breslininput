#ifndef ANIMATIONSTATEMACHINE_H
#define ANIMATIONSTATEMACHINE_H
#include "animationState.h"

class OgreAnimation;

class AnimationStateMachine
{
private:
  //a pointer to the agent that owns this instance
  OgreAnimation*   m_pOwner;
  OgreAnimationState*    m_pCurrentState;

  //a record of the last state the agent was in
  OgreAnimationState*   m_pPreviousState;

  //this is called every time the FSM is updated
  OgreAnimationState*   m_pGlobalState;

public:

  AnimationStateMachine(OgreAnimation* owner):m_pOwner(owner),
	                               m_pCurrentState(0),
                                   m_pPreviousState(0),
                                   m_pGlobalState(0)
  {}

  virtual ~AnimationStateMachine(){}

  //use these methods to initialize the FSM
  void setCurrentState(OgreAnimationState* s){m_pCurrentState = s;}
  void setGlobalState(OgreAnimationState* s) {m_pGlobalState = s;}
  void setPreviousState(OgreAnimationState* s){m_pPreviousState = s;}

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
  void  changeState(OgreAnimationState* pNewState)
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

  OgreAnimationState*  currentState()  const{return m_pCurrentState;}
  OgreAnimationState*  globalState()   const{return m_pGlobalState;}
  OgreAnimationState*  previousState() const{return m_pPreviousState;}
};
#endif



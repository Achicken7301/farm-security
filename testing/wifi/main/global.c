#include "global.h"
TransitionState currentState;

void setState(TransitionState state)
{
  printf("Current State is:\t%d\n", state);
  currentState = state;
}

TransitionState getState() { return currentState; }
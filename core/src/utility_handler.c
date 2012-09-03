#include<stdio.h>
#include"utility_handler.h"

//All the transition Functions

static int GotKey (void) 
{
  printf("\n INIT: %d , event = %d", state,event);
  state = ST_AUTH;
  return state;
}
static int FsmError (void) 
{ 
  printf("\nGot a error in FSM state: %d , event = %d", state,event);
  state = ST_TERM;
}
static int Auth(void)
{
  printf("\n Auth : %d , event = %d", state,event);
  state = ST_READ;
  return state;
}
static int ReadData(void)
{
  printf("\n Read data : %d , event = %d", state,event);
  state = ST_END;
  return state;
}
static int EndComm(void)
{
  printf("\n End Communication: %d , event = %d", state,event);
  state = ST_TERM;
  return state;
}


int GetNextEvent()
{
  return EV_KEYPRESS;
}


// Main Method
int main()
{
  int i=0;
  state = ST_INIT;
  while (state != ST_TERM) {
        event = GetNextEvent();
        for (i = 0; i < TRANS_COUNT; i++) {
          if ((state == trans[i].st)) {
            if ((event == trans[i].ev)) {
              state = (trans[i].fn)();
              break;
            }
          }
        }
  }
  return 0;
}


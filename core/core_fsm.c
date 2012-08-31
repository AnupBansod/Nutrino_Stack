#include<stdio.h>

//Global Data-Structures
typedef struct {
      int st;
      int ev;
      int (*fn)(void);
} tTransition;
int state,event;

// All the states

#define ST_ANY              -1
#define ST_INIT              0
#define ST_ERROR             1
#define ST_TERM              2
#define ST_AUTH              3
#define ST_READ              4
#define ST_END               5

//All the Events

#define EV_ANY              -1
#define EV_KEYPRESS       5000
#define EV_MOUSEMOVE      5001
#define EV_USERPASS       5002
#define EV_READDATA       5003
#define EV_ENDSIG         5004

//All the transition Functions


static int GotKey (void) 
{
  printf("\n INIT: %d , event = %d", state,event);
  state=ST_AUTH;
  return state;
}
static int FsmError (void) 
{ 
  printf("\nGot a error in FSM state: %d , event = %d", state,event);
  state=ST_TERM;
}
static int Auth(void)
{
  printf("\n Auth : %d , event = %d", state,event);
  state=ST_READ;
  return state;
}
static int ReadData(void)
{
  printf("\n Read data : %d , event = %d", state,event);
  state=ST_END;
  return state;
}
static int EndComm(void)
{
  printf("\n End Communication: %d , event = %d", state,event);
  state=ST_TERM;
  return state;
}


tTransition trans[] = {
        { ST_INIT, EV_KEYPRESS, &GotKey},
        { ST_AUTH, EV_KEYPRESS, &Auth},
        { ST_READ, EV_KEYPRESS, &ReadData},
        { ST_END, EV_KEYPRESS, &EndComm},
        { ST_ANY, EV_KEYPRESS, &FsmError}
};
#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

int GetNextEvent()
{
  return EV_KEYPRESS;
}
// Main Method
int main()
{
  int i=0;
  state = ST_INIT;
  printf("%d",TRANS_COUNT);
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
        printf("State = %d %d , event = %d",state,ST_TERM,event);
  }
  return 0;
}


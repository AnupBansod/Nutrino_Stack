#include<stdio.h>
#include<queue_manager.h>
#include<core_fsm.h>

int state,event;

#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

tTransition trans[] = {
        { ST_INIT, EV_READ_FROM_DEVICE, &GotKey},
        { ST_AUTH, EV_READ_FROM_DEVICE, &Auth},
        { ST_READ, EV_READ_FROM_DEVICE, &ReadData},
        { ST_END, EV_READ_FROM_DEVICE, &EndComm},
        { ST_ANY, EV_READ_FROM_DEVICE, &FsmError}
};

//All the transition Functions

int GotKey (struct msgbuff msg) 
{
  printf("\n INIT: %d , event = %d", state,event);
  msg.msg_state = ST_AUTH;
  msg.msg_event = EV_READ_FROM_DEVICE;
  message_queue_write( QUEUE_WRITE_TO_APPLICATION, &msg);
  return 0;
}

int FsmError (struct msgbuff msg) 
{ 
  printf("\nGot a error in FSM state: %d , event = %d", state,event);
  msg.msg_state = ST_TERM;
  msg.msg_event = EV_READ_FROM_DEVICE;
  message_queue_write( QUEUE_WRITE_TO_APPLICATION, &msg);
  return 0;
}

int Auth(struct msgbuff msg)
{
  msg.msg_state = ST_READ;
  msg.msg_event = EV_READ_FROM_DEVICE;
  message_queue_write( QUEUE_WRITE_TO_APPLICATION, &msg);
  printf("\n Auth : %d , event = %d", state,event);
  return 0;
}

int ReadData(struct msgbuff msg)
{
  printf("\n Read data : %d , event = %d", state,event);
  msg.msg_state = ST_READ;
  msg.msg_event = EV_READ_FROM_DEVICE;
  message_queue_write( QUEUE_WRITE_TO_APPLICATION, &msg);
  return 0;
}

int EndComm(struct msgbuff msg)
{
  printf("\n End Communication: %d , event = %d \n", state,event);
  msg.msg_state = ST_TERM;
  msg.msg_event = EV_READ_FROM_DEVICE;
  message_queue_write( QUEUE_WRITE_TO_APPLICATION, &msg);
  return state;
}

struct msgbuff GetMessageStateEvent()
{
  struct msgbuff msg;
  message_queue_read( QUEUE_READ_FROM_DEVICE, &msg);
  printf("%s\n", msg.msg_text);
  if(msg.msg_type == 1)
  {
      state = msg.msg_state;
      event = msg.msg_event;
  }
  if(msg.msg_type == 2)
  {
      //Do some stuff
  }
  if(msg.msg_type == 3)
  {
      //Do some stuff
  }
}

// Main Method
int main()
{
    //Initialise the required things
    int i=0;
    state = ST_INIT;
    struct msgbuff msg;
    //Infinite while loop waiting for messages
    while (state != ST_TERM) {
        
        //Get the Message Sate and Event 
        //Also this would be a blocker
        msg = GetMessageStateEvent();

        //One State Table
        for (i = 0; i < TRANS_COUNT; i++) {
            //Check for the state of message
          if ((state == trans[i].st)) {
            //Check for the event in the msg 
            if ((event == trans[i].ev)) {
                // call the function according to the next state
              state = (trans[i].fn)(msg);
              break;
            }
          }
        }
        
        //Next State Table


    
  }
  return 0;
}


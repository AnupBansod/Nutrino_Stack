#include<stdio.h>
#include<queue_manager.h>
#include<core_fsm.h>

long state,event,message_type;
struct msgbuff msg;

#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

tTransition trans[] = {
        { ST_INIT, EV_READ_FROM_DEVICE, &GotKey},
        { ST_AUTH, EV_READ_FROM_DEVICE, &Auth},
        { ST_READ, EV_READ_FROM_DEVICE, &ReadData},
        { ST_END, EV_READ_FROM_DEVICE, &EndComm},
        { ST_ANY, EV_READ_FROM_DEVICE, &FsmError}
};

tTransition glow_led[] = {
        { ST_GLOW_LED_INIT, EV_SEND_TO_CORE_NO_ACK , &Init_led },
        { ST_GLOW_LED_INIT, EV_SEND_TO_CORE , &Init_led },
        { ST_GLOW_LED_INIT, EV_SEND_TO_CORE_DEV_ACK , &Init_led },
        { ST_GLOW_LED_SEND_TO_DEVICE, EV_SEND_TO_DEVICE_NO_ACK, &Send_no_ack}
};
//All the transition Functions
int Init_led(struct msgbuff m)
{
    printf("\nCORE: State : %ld Event %ld \n Message :%s\n\n\n", msg.msg_state, msg.msg_event, msg.msg_text);
    switch(m.msg_event)
    {
        case EV_SEND_TO_CORE_NO_ACK:

            m.msg_state = ST_GLOW_LED_SEND_TO_DEVICE;
            m.msg_event = EV_SEND_TO_DEVICE_NO_ACK;
            message_queue_write( QUEUE_CORE, &m);

            printf("\nCORE 1 : State : %ld Event %ld \n Message :%s\n\n\n", msg.msg_state, msg.msg_event, msg.msg_text);
            break;

        case EV_SEND_TO_CORE:

            m.msg_state = ST_GLOW_LED_SEND_ACK;
            m.msg_event = EV_SEND_TO_APP;
            message_queue_write( QUEUE_APPLICATION, &m);

            printf("\nCORE 2.1: State : %ld Event %ld \n Message :%s\n\n\n", msg.msg_state, msg.msg_event, msg.msg_text);

            m.msg_state = ST_GLOW_LED_SEND_TO_DEVICE;
            m.msg_event = EV_SEND_TO_DEVICE_NO_ACK;
            message_queue_write( QUEUE_DEVICE, &m);

            printf("\nCORE 2.2 : State : %ld Event %ld \n Message :%s\n\n\n", msg.msg_state, msg.msg_event, msg.msg_text);
            break;

        case EV_SEND_TO_CORE_DEV_ACK:
            m.msg_state = ST_GLOW_LED_SEND_TO_DEVICE;
            m.msg_event = EV_SEND_TO_DEVICE;
            message_queue_write( QUEUE_DEVICE, &m);
    
            printf("\nCORE 3: State : %ld Event %ld \n Message :%s\n\n\n", msg.msg_state, msg.msg_event, msg.msg_text);
            break;
        default:
            printf("\nUnknown Error\n");
      }
    return 0;
}
int Send_no_ack(struct msgbuff m)
{
    printf("\nSending NO ack back to app\n");
    return 0;
}

int GotKey (struct msgbuff msg) 
{
  return 0;
}

int FsmError (struct msgbuff msg) 
{ 
  printf("\nGot a error in FSM state: %d , event = %d", state,event);
  msg.msg_state = ST_TERM;
  msg.msg_event = EV_READ_FROM_DEVICE;
  message_queue_write( QUEUE_CORE, &msg);
  return 0;
}

int Auth(struct msgbuff msg)
{
  return 0;
}

int ReadData(struct msgbuff msg)
{
  return 0;
}

int EndComm(struct msgbuff msg)
{
  return state;
}

struct msgbuff GetMessageStateEvent()
{
    message_queue_read( QUEUE_CORE, &msg);
 //   printf("\nGot message [ Inside Core ]\n");
    printf("\n CORE: State : %ld Event %ld \n Message :%s\n\n\n", msg.msg_state, msg.msg_event, msg.msg_text);
//    printf("\n In GetSTEV State : %ld Event %ld session: %ld\n", msg.msg_state, msg.msg_event, msg.msg_session_id);
 //   printf("%s\n", msg.msg_text);
    if(msg.msg_type == MSG_TYPE_GLOW_LED)
    {
        state = msg.msg_state;
        event = msg.msg_event;
  //      printf("\nPutting it into Device Queue\n");
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
    message_queue_init(QUEUE_CORE);
    message_queue_init(QUEUE_APPLICATION);
    message_queue_init(QUEUE_DEVICE);
    //Infinite while loop waiting for messages
    while (state != ST_TERM) {
        
        //Get the Message Sate and Event 
        //Also this would be a blocker
        GetMessageStateEvent();
//        printf("\nCore State : %d Event %d \n", state,event);
        //One State Table
        if(message_type == 1)
        {
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
        }
        if(message_type == MSG_TYPE_GLOW_LED)

        for (i = 0; i < TRANS_COUNT; i++) {
            //Check for the state of message
          if ((state == glow_led[i].st)) {
            //Check for the event in the msg 
            if ((event == glow_led[i].ev)) {
                // call the function according to the next state
              state = (glow_led[i].fn)(msg);
              break;
            }
          }
        }
        //Next State Table

  }
  return 0;
}


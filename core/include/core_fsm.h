
//Global Data-Structures
typedef struct {
      int st;
      int ev;
      int (*fn)(struct msgbuff);
} tTransition;

// All the states

#define ST_ANY              -1
#define ST_ERROR             1
#define ST_TERM              2
#define ST_AUTH              3
#define ST_READ              4
#define ST_END               5

//States for LED
#define ST_INIT                    12301
#define ST_SEND_TO_DEVICE          12302
#define ST_SEND_ACK                12303
#define ST_ACK_SENDING             12305

//All the Events

#define EV_ANY              -1
#define EV_READ_FROM_DEVICE         5000
#define EV_WRITE_TO_DEVICE          5001
#define EV_READ_FROM_APP            5002
#define EV_READDATA                 5003
#define EV_ENDSIG                   5004


//Events for LED
#define EV_SEND_TO_CORE_NO_ACK      12351
#define EV_SEND_TO_DEVICE           12352
#define EV_SEND_TO_CORE             12353
#define EV_SEND_TO_APP              12354
#define EV_SEND_TO_CORE_DEV_ACK     12355
#define EV_SEND_TO_DEVICE_NO_ACK    12352

//All the transition Functions

int GotKey (struct msgbuff) ;
int FsmError (struct msgbuff) ;
int Auth (struct msgbuff) ;
int ReadData (struct msgbuff) ;
int EndComm (struct msgbuff) ;

int GetNextEvent();



//STATE_LED_CALL_BACKS
int Init_led(struct msgbuff) ;
int Send_ack(struct msgbuff) ;

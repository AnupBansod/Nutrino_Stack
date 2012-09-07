
//Global Data-Structures
typedef struct {
      int st;
      int ev;
      int (*fn)(struct msgbuff);
} tTransition;

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
#define EV_READ_FROM_DEVICE       5000
#define EV_WRITE_TO_DEVICE        5001
#define EV_READ_FROM_APP          5002
#define EV_READDATA       5003
#define EV_ENDSIG         5004

//All the transition Functions

int GotKey (struct msgbuff) ;
int FsmError (struct msgbuff) ;
int Auth (struct msgbuff) ;
int ReadData (struct msgbuff) ;
int EndComm (struct msgbuff) ;

int GetNextEvent();

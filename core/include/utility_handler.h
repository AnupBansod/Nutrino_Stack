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


static int GotKey (void);
static int FsmError (void); 
static int Auth(void);
static int ReadData(void);
static int EndComm(void);

tTransition trans[] = {
        { ST_INIT, EV_KEYPRESS, &GotKey},
        { ST_AUTH, EV_KEYPRESS, &Auth},
        { ST_READ, EV_KEYPRESS, &ReadData},
        { ST_END, EV_KEYPRESS, &EndComm},
        { ST_ANY, EV_KEYPRESS, &FsmError}
};
#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

int GetNextEvent();

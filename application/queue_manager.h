#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE     128

#define QUEUE_DEVICE      10201
#define QUEUE_APPLICATION 10202
#define QUEUE_UTILITY     10203
#define QUEUE_CORE        10204

struct msgbuff
{
    long    msg_session_id;
    long    msg_type;
    long    msg_state;
    long    msg_event;
    char    msg_text[MAXSIZE];
};

int message_queue_init(key_t key);
int message_queue_write(key_t key, struct msgbuff *sbuf );
int message_queue_read(key_t key, struct msgbuff *sbuf);

//Types of message

#define MSG_TYPE_DUMY_MSG           43400
#define MSG_TYPE_GLOW_LED           0

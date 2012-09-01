#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE     128

#define QUEUE_READ_FROM_DEVICE      10201
#define QUEUE_WRITE_TO_DEVICE       10202
#define QUEUE_READ_FROM_APPLICATION 10203
#define QUEUE_READ_TO_APPLICATION   10204
#define QUEUE_READ_FROM_UTILITY     10205
#define QUEUE_READ_TO_UTILITY       10206

struct msgbuff
{
    long    mtype;
    char    mtext[MAXSIZE];
};

int message_queue_init(key_t key);
int message_queue_write(key_t key, struct msgbuff *sbuf );
int message_queue_read(key_t key, struct msgbuff *sbuf);

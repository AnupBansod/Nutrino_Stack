#include <queue_manager.h>
#include <strings.h>
int message_queue_init(key_t key)
{
     int msgflg = IPC_CREAT | 0666;
     if ((msgget(key, msgflg )) < 0) 
     {
       printf("\nMessage Queue - Unable to initialised for key : %d \n",(int) key);
       return -1;
     }
     printf("\nMessage Queue Intialised with key %d \n", (int)key);
     return 0;
}

int message_queue_write(key_t key, struct msgbuff *sbuf )
{
    int msqid;
    size_t buflen;
    if ((msqid = msgget(key, 0666 )) < 0){   //Get the message queue ID for the given key
      printf("\nMessage Queue not initialised previously for key : %d \n",(int) key);
      return -1;
    }

    buflen = strlen(sbuf->msg_text) + 1 ;

    if (msgsnd(msqid, sbuf, buflen, IPC_NOWAIT) < 0)
    {
        printf ("\nQueue Failure - Send : %d, %d, %s, %d\n", msqid, (int)sbuf->msg_type, sbuf->msg_text, (int)buflen);
        return -1;
    }
    printf("\n Message is written to the queue with key : %d , Message : %s \n", (int)key, sbuf->msg_text);
    return 0;
  
}

int message_queue_read(key_t key, struct msgbuff *sbuf)
{
   int msqid;
   size_t buflen;
   if ((msqid = msgget(key, 0666)) < 0){
      printf("\nMessage Queue not initialised previously for key : %d ",(int) key);
      return -1;
   }

   buflen = strlen(sbuf->msg_text) + 1 ;

   if (msgrcv(msqid, sbuf, MAXSIZE, 1, 0) < 0)
   {
        printf ("\nQueue Failure - Send : %d, %d, %s, %d\n", msqid, (int)sbuf->msg_type, sbuf->msg_text, (int)buflen);
        return -1;
   }

   printf("\nMessage is read from the queue with key : %d , Message : %s \n", (int)key, sbuf->msg_text);
   return 0;
    
}

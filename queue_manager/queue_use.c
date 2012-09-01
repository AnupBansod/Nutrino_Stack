#include"queue_manager.h"
int main()
{
    struct msgbuff sbuf,sbuf2;
    message_queue_init(QUEUE_READ_FROM_DEVICE);
    //Message Type
    sbuf.mtype = 1;
    printf("Enter a message to add to message queue %d: ", QUEUE_READ_FROM_DEVICE);
    scanf("%[^\n]",sbuf.mtext);
    getchar();

    message_queue_write( QUEUE_READ_FROM_DEVICE, &sbuf );
     
    message_queue_read( QUEUE_READ_FROM_DEVICE, &sbuf2);
    printf("%s\n", sbuf2.mtext);

    return 0;
}


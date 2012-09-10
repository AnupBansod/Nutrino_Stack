#include<stdio.h>
#include<time.h>
#include<queue_manager.h>
int main()
{
    struct msgbuff msg;
    while(1)
    {
       message_queue_read(QUEUE_DEVICE, &msg);
       printf("\nDevice Wrapper Read \n : State : %ld Event %ld \n Message :%s\n\n\n", msg.msg_state, msg.msg_event, msg.msg_text);

       msg.msg_type = MSG_TYPE_GLOW_LED;
       strcpy(msg.msg_text, "Sending Data to Core I am sending an ack from device \n");
       message_queue_write(QUEUE_CORE,&msg);
       printf("\nDevice Wrapper Send \n : State : %ld Event %ld \n Message :%s\n\n\n", msg.msg_state, msg.msg_event, msg.msg_text);
    }
}


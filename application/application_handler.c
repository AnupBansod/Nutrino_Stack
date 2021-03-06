#include <queue_manager.h>
//#include <strings.h>
#include <core_fsm.h>
int glow_led()
{
    struct msgbuff msg;
    msg.msg_type = MSG_TYPE_GLOW_LED;
    strcpy(msg.msg_text, "Sending Data to core I am NOT going to get an Ack \n");
    msg.msg_session_id = 34567; // **TODO** this will be a random number generated by some api
    msg.msg_state = ST_INIT;
    msg.msg_event = EV_SEND_TO_CORE_NO_ACK;
    message_queue_write(QUEUE_CORE,&msg);
    printf("\nApp-Handler: State : %ld Event %ld \n Message :%s\n\n\n", msg.msg_state, msg.msg_event, msg.msg_text);
    return 0;
}

int glow_led_with_fsm_ack()
{
    struct msgbuff msg;
    msg.msg_type = MSG_TYPE_GLOW_LED;
    strcpy(msg.msg_text, "Sending Data to Core I am going to get an Ack from FSM\n");
    msg.msg_session_id = 34567; // **TODO** this will be a random number generated by some api
    msg.msg_state = ST_INIT;
    msg.msg_event = EV_SEND_TO_CORE;
    message_queue_write(QUEUE_CORE,&msg);
    printf("\nApp-Handler: State : %ld Event %ld \n Message :%s\n\n\n", msg.msg_state, msg.msg_event, msg.msg_text);
    message_queue_read(QUEUE_APPLICATION, &msg);
    printf("\nApp-Handler: State : %ld Event %ld \n Message :%s\n\n\n", msg.msg_state, msg.msg_event, msg.msg_text);
    return 0;
}

int glow_led_with_board_ack()
{
    struct msgbuff msg;
    msg.msg_type = MSG_TYPE_GLOW_LED;
    strcpy(msg.msg_text, "Sending Data to Core I am going to get an Ack from FSM\n");
    msg.msg_session_id = 34567; // **TODO** this will be a random number generated by some api
    msg.msg_state = ST_INIT;
    msg.msg_event = EV_SEND_TO_CORE_DEV_ACK;
    message_queue_write(QUEUE_CORE,&msg);
    printf("\nApp-Handler: State : %ld Event %ld \n Message :%s\n\n\n", msg.msg_state, msg.msg_event, msg.msg_text);
    message_queue_read(QUEUE_APPLICATION, &msg);
    printf("\nApp-Handler: State : %ld Event %ld \n Message :%s\n\n\n", msg.msg_state, msg.msg_event, msg.msg_text);
    return 0;
 }

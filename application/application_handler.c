int glow_led()
{
    struct msgbuff msg;
    msg.msg_type=MSG_TYPE_GLOW_LED;
    msg.msg_text="Ignore ME";
    msg.msg_session_id = 34567 // **TODO** this will be a random number generated by some api
    msg.msg_state = ST_GLOW_LED_INIT;
    msg.msg_action = EV_SEND_TO_CORE;
    message_queue_write(QUEUE_READ_FROM_DEVICE,&msg);
}
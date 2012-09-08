int callback_from_device(const char *raw_buff, int size) 
{
  struct msgbuff *msg = malloc(sizeof(*msg));
  memcpy(msg->mtext,raw_buff,size); 
 
  msg->type=1;
  return message_queue_write( QUEUE_READ_FROM_DEVICE, &sbuf );
}

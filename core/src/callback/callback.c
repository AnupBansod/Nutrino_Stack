/*int callback_from_device(const char *raw_buff, int size) 
{
//  struct msgbuff *msg = malloc(sizeof(*msg));
//  memcpy(msg->mtext,raw_buff,size); 
 
  msg->type=1;
  return message_queue_write( QUEUE_READ_FROM_DEVICE, &sbuf );
}*/

int main()
{
     struct msgbuff msg;
     int i;

     memcpy(msg.mtext, raw_buff, size); 
     msg.mtype = 1;
#if 1
     for (i = 0; i < size; i++)
        printf("%x ", msg.mtext[i] & 0xFF);
#endif
     return message_queue_write(QUEUE_READ_FROM_DEVICE, &msg);

}

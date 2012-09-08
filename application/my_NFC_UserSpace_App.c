#include<sdtio.h>
#include<queue_manager.h>
int main()
{
    int return_value=glow_led();    
    if(return_value=0)
        printf("Message Sent to Core_FSM with success");
}


#include<stdio.h>
#include<queue_manager.h>
int main()
{
    int return_value=glow_led();    
    if(0 == return_value)
        printf("\nMessage Sent to Core_FSM with success\n");
    else 
        printf("\nSome error in sending the message to core fms\n");
}


#include<stdio.h>
#include<time.h>
#include<queue_manager.h>
int main()
{
    while(1)
    {
        int choice=0,return_value;
        printf("\nWhat do you want to do with your life ??\n");
        printf("\n1.Glow LED without an Ack\n 2.Glow LED with an Ack From FSM\n 3.Glow LED with an Ack From Board");
        scanf("%d",&choice);
        switch(choice)
        {
            case 1:
                    return_value=glow_led();    
                    if(0 == return_value)
                        printf("\nMessage Sent to Core_FSM with success\n");
                    else
                        printf("\nSome error in sending the message to core fms\n");
                    sleep(15);
                    break;
            case 2:
                    return_value=glow_led_with_fsm_ack();    
                    if(0 == return_value)
                        printf("\nMessage Sent to Core_FSM with success and recieved ack from fsm\n");
                    else
                        printf("\nSome error in sending the message to core fms\n");
                    sleep(15);
                    break;
            case 3:
                    return_value=glow_led_with_board_ack();
                    if(0 == return_value)
                        printf("\nMessage Sent to Core_FSM with success\n");
                    else
                        printf("\nSome error in sending the message to core fms\n");
                    sleep(15);
                    break;
            default:
                    printf("\nEnter a valid input!! What is your problem \n");

        }
    }
}


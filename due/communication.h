#include "motor.h"



void handlepacket(char var, String returned){
    Serial.print("received packet (");
    Serial.print(var);
    Serial.print("): ");

    switch(var){
        case vmotorcontrol:{
            motorcontrol = returned.charAt(0);
        }
        
        default:{
            break;
        }
    }
}
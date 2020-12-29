#include "motor.h"



void handlepacket(char var, String returned){
    Serial.print("received packet (");
    Serial.print(var);
    Serial.print("): ");

    switch(var){
        case startup:{
            resetboard;
        }
        case vmotorcontrol:{
            motorcontrol = returned.charAt(0);
            break;
        }

        Serial.println(returned.toInt());
        case setdikte:{
            dikte = returned.toInt();
            break;
        }

        case setdiepte:{
            diepte = returned.toInt();
            break;
        }

        case phoogte:{
            hoogte = returned.toInt();
            break;
        }

        case pmes:{
            mes = returned.toInt();
            break;
        }

        case pmaxspd:{
            maxspd = returned.toInt();
            maxdif = 100 - maxspd;
            break;
        }

        case pminspd:{
            minspd = returned.toInt();
            break;
        }
        
        default:{
            break;
        }
    }
}
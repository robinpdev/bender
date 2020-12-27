#include "util.h"
#define cp Serial3 //current communication port
#include "metacom.h"

#include "encoder.h"

void setup()
{
    Serial.begin(115200); // start de seriële communicatie op een snelheid van 115200 bits per seconde.
    Serial.println("startup");

    pinMode(buzpin, OUTPUT);
    beep();

    Serial3.begin(115200);
    Serial3.write('s');

    enc1_start();
    enc2_start();

    send('a', 't');
    send('b', 12345);
}

void loop(){

    if(enc1.pos != enc1_prevpos){
        Serial.print("encoder 1: ");
        Serial.println(enc1.pos);
        enc1_prevpos = enc1.pos;
    }

    delay(5);
    updatepackets();
}




//no more global variable definitions from this point!
#include "communication.h"
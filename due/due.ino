#include "util.h"
#define cp Serial3 //current communication port
#include "communication.h"


void setup()
{
    Serial.begin(115200); // start de seriële communicatie op een snelheid van 115200 bits per seconde.
    Serial.println("startup");

    pinMode(buzpin, OUTPUT);
    beep();

    Serial3.begin(115200);
    Serial3.write('s');

    send('a', 't');
    send('b', 12345);
}

void loop(){
    delay(5);
    updatepackets();
}
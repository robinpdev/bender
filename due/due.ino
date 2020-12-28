#include <arduino.h>

#define pinon(pin) digitalRead(pin)

#define startup 's'
#define vmotorcontrol 'm'
#define apos 'a'
#define bpos 'b'

#include "util.h"
#define cp Serial3 //current communication port
#include "metacom.h"
#include "encoder.h"
#include "motor.h"

const short bup = 7, bdown = 6;
bool bupstate = false, bdownstate = false;

void setup()
{
    Serial.begin(115200); // start de seriële communicatie op een snelheid van 115200 bits per seconde.
    Serial.println("startup");

    pinMode(buzpin, OUTPUT);
    beep();

    Serial3.begin(115200);
    Serial3.write('s');

    enca_start();
    encb_start();

    send(startup);
}

void loop()
{
    while (true)
    {
        if (pinon(bup) == HIGH && !bupstate)
        {
            bupstate = true;
            motorforward();
        }
        else if (pinon(bdown) == HIGH && !bdownstate)
        {
            bdownstate = true;
            motorback();
        }
        else if ((bupstate || bdownstate) && pinon(bdown) == LOW && pinon(bup) == LOW)
        {
            bupstate = false;
            bdownstate = false;
            motorstop();
        }

        if(enca.gotref && encb.gotref){
            motorstop();
            break;
        }
        updatepackets();
        delay(5);
    }

    Serial.println("got the refsss");

    while(true){
        if (enca.pos != enca.prevpos)
        {
            detachInterrupt(digitalPinToInterrupt(enca.chr));
            Serial.print("encoder 1: ");
            Serial.println(enca.pos);
            send(apos, enca.pos);
            enca.prevpos = enca.pos;
        }

        if (encb.pos != encb.prevpos)
        {
            detachInterrupt(digitalPinToInterrupt(encb.chr));
            Serial.print("encoder 2: ");
            Serial.println(encb.pos);
            encb.prevpos = encb.pos;
        }
    }

    delay(5000);
    
}

//no more global variable definitions from this point!
#include "communication.h"
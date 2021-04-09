#include <arduino.h>

#define pinon(pin) digitalRead(pin)

#include "packettypes.h"

#include "util.h"
#define cp Serial3 //current communication port
#include "metacom.h"
#include "encoder.h"
#include "motor.h"

const short bup = 7, bdown = 6;
bool bupstate = false, bdownstate = false;

int dikte = -1;
int diepte = -1;
int hoogte = 2000;
int mes = 32;
int maxspd = 64, minspd = 40;

short maxdif = 100 - maxspd; //NIET AANKOMEN

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

    send(phoogte, hoogte); send(pmes, mes); send(pmaxspd, maxspd); send(pminspd, minspd);
	
}

void loop()
{
    while (true)
    {
        if (pinon(bup) == HIGH && !bupstate)
        {
            bupstate = true;
            motorupward();
        }
        else if (pinon(bdown) == HIGH && !bdownstate)
        {
            bdownstate = true;
            motordownward();
        }
        else if ((bupstate || bdownstate) && pinon(bdown) == LOW && pinon(bup) == LOW)
        {
            bupstate = false;
            bdownstate = false;
            motorstop();
        }

        if(enca.gotref && encb.gotref){
            motorstop();
            bupstate = false;
            bdownstate = false;
            break;
        }
        updatepackets();
        delay(5);
    }

    Serial.println("got the refsss");
    send(vstate, 1);

    while(true){
        updatepackets();
        if(diepte != -1 && dikte != -1){
            break;
        }
        delay(20);
    }

    int doel = hoogte - diepte + dikte + mes;
    Serial.print("doel: ");
    Serial.println(doel);
    Serial.println("ready to plooi");
    send(vstate, 2);
    beep(26);
    delay(26);
    beep(26);

    while(true){
        
        /*if (pinon(bup) == HIGH && !bupstate)
        {
            bupstate = true;
            motorupward();
        }
        else if (pinon(bdown) == HIGH && !bdownstate)
        {
            bdownstate = true;
            motordownward();
        }*/

        if (pinon(bup) == HIGH && !bupstate)
        {
            bupstate = true;
            motorupward();
        }

        if(pinon(bdown) == HIGH){
            bdownstate = true;
            short downspeeda = 0;
            if(enca.pos - doel > 400){
                downspeeda = maxspd * 2.55; //van procent naar 0-255
            }else{ 
                downspeeda = 38 * 2.55; //vaste snelheid van 38% wanneer dicht bij doel
            }
            motora.downward(downspeeda);
            
            //compensation routine
            //posb = posa + dif
            //<-> dif = posb - posa
            int posdif = encb.pos - enca.pos; //positieverschil
            float difspeed = 0;
            if(posdif > 1){
                difspeed = min(map(posdif, 0, 400, 10, maxdif), maxdif);
            }else if(posdif < 1){
                difspeed = max(map(posdif, 0, -400, -10, -maxdif), -maxdif);
            }else{
                //de snelheid moet exact die van de master zijn
            }
            motorb.downward(downspeeda + difspeed * 2.55);
            Serial.print("gap: ");
            Serial.println(posdif);
            Serial.print("b speed: ");
            Serial.println(maxspd * 2.55 + difspeed * 2.55);
        }

        if(enca.pos - doel <= 1){
            motorstop();
            break;
        }

        if ((bupstate && pinon(bup) == LOW) || (bdownstate && pinon(bdown) == LOW))
        {
            bupstate = false;
            bdownstate = false;
            motorstop();
        }

        

        if(enca.pos != enca.prevpos){
            enca.prevpos = enca.pos;
            send(apos, enca.pos);
        }

        if(encb.pos != encb.prevpos){

        }
        
        updatepackets();
        delay(10);
    }

    beep(648);
    Serial.println(enca.pos - doel);

    while(true){
        delay(5000);
    }
    
}

//no more global variable definitions from this point!
#include "communication.h"
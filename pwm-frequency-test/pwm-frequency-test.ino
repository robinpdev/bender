#include <tc_defs.h>
#include <tc_lib.h>

#include <pwm_defs.h>
#include <pwm_lib.h>
#include "encoder.h"
#include "motor.h"

//#include <pdueclasses.h>

using namespace arduino_due::pwm_lib;

const short bup = 7, bdown = 6;
bool bupstate = false, bdownstate = false;

pwm<pwm_pin::PWMH1_PA19> pwm_pin64; //42
int pwm_period = 1000000;           //periode
int pwm_duty = 250000;              //dutycycle

int frequency = 100;
int duty = 60;

void setup()
{
    // put your setup code here, to run once:
    pinMode(bup, INPUT);
    pinMode(bdown, INPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    rotenc_start();
    rotenc_refreceived();
    Serial.begin(115200);
    Serial.println("startup");
}

void loop()
{
    // put your main code here, to run repeatedly:
    if (digitalRead(bup) == HIGH && !bupstate)
    {
        bupstate = true;
        digitalWrite(8, LOW);
        digitalWrite(9, HIGH);
    }
    else if (digitalRead(bdown) == HIGH && !bdownstate)
    {
        bdownstate = true;
        digitalWrite(9, LOW);
        pwm_pin64.stop();
        pwm_pin64.start((1.0 / frequency) * pow(10, 8), (1.0 / frequency) * pow(10, 8) * (duty / 100.0));
        Serial.println("down");
    }
    else if ((bupstate && digitalRead(bup) == LOW) || (bdownstate && digitalRead(bdown) == LOW))
    {
        bupstate = false;
        bdownstate = false;
        digitalWrite(9, LOW);
        pwm_pin64.stop();
    }

    if (rotenc.pos != rotenc.prevpos)
    {
        rotenc.prevpos = rotenc.pos;
        frequency = 100 + rotenc.pos;
        if (frequency <= 0)
        {
            frequency = 1;
        };
        Serial.print("frequency: ");
        Serial.println(frequency);
        if (bdownstate)
        {
            pwm_pin64.stop();
            pwm_pin64.start((1.0 / frequency) * pow(10, 8), (1.0 / frequency) * pow(10, 8) * (duty / 100.0));
        }
    }

    delay(10);
}
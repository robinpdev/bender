#ifndef MOTOR_H
#define MOTOR_H

#define geen 'o'
#define beide 'c'
#define controla 'a'
#define controlb 'b'
char motorcontrol = beide;

class motor{
    public:
    short forpin, backpin;
    unsigned char speed = 255;
    bool enabled = true;
    char state = 'o';

    motor(short mforpin, short mbackpin){
        forpin = mforpin;
        backpin = mbackpin;
        pinMode(forpin, OUTPUT);
        pinMode(backpin, OUTPUT);
        stop();
    }

    void forward(){
        state = 'f';
        digitalWrite(backpin, LOW);
        analogWrite(forpin, speed);
    }
    void back(){
        state = 'b';
        digitalWrite(forpin, LOW);
        analogWrite(backpin, speed);
    }
    void stop(){
        state = 'o';
        digitalWrite(forpin, LOW);
        digitalWrite(backpin, LOW);
    }

    void forward(unsigned char inspeed){
        state = 'f';
        digitalWrite(backpin, LOW);
        analogWrite(forpin, inspeed);
    }
    void back(unsigned char inspeed){
        state = 'b';
        digitalWrite(forpin, LOW);
        analogWrite(backpin, inspeed);
    }
};

motor motora(9, 8);
motor motorb(12, 11);

void motorforward(){
    if(motorcontrol == beide){
        Serial.println("forward");
        motora.forward();
        motorb.forward();
    }else if(motorcontrol == controla){
        motora.forward();
    }else if (motorcontrol == controlb){
        motorb.forward();
    }
}

void motorback(){
    if(motorcontrol == beide){
        Serial.println("back");
        motora.back();
        motorb.back();
    }else if(motorcontrol == controla){
        motora.back();
    }else if (motorcontrol == controlb){
        motorb.back();
    }
}

void motorstop(){
    Serial.println("stop");
    motora.stop();
    motorb.stop();
}
#endif
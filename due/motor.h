#ifndef MOTOR_H
#define MOTOR_H

#define geen 'o'
#define beide 'c'
#define controla 'a'
#define controlb 'b'
char motorcontrol = beide;

class motor{
    public:
    short uppin, downpin;
    unsigned char speed = 255;
    bool enabled = true;
    char state = 'o';

    motor(short mforpin, short mbackpin){
        uppin = mforpin;
        downpin = mbackpin;
        pinMode(uppin, OUTPUT);
        pinMode(downpin, OUTPUT);
        stop();
    }

    void upward(){
        state = 'f';
        digitalWrite(downpin, LOW);
        analogWrite(uppin, speed);
    }
    void downward(){
        state = 'b';
        digitalWrite(uppin, LOW);
        analogWrite(downpin, speed);
    }
    void stop(){
        state = 'o';
        digitalWrite(uppin, LOW);
        digitalWrite(downpin, LOW);
    }

    void upward(float inspeed){
        state = 'f';
        digitalWrite(downpin, LOW);
        analogWrite(uppin, inspeed);
    }
    void downward(float inspeed){
        state = 'b';
        digitalWrite(uppin, LOW);
        analogWrite(downpin, inspeed);
    }
};

motor motora(9, 8);
motor motorb(12, 11);

void motorupward(){
    if(motorcontrol == beide){
        Serial.println("forward");
        motora.upward();
        motorb.upward();
    }else if(motorcontrol == controla){
        motora.upward();
    }else if (motorcontrol == controlb){
        motorb.upward();
    }
}

void motordownward(){
    if(motorcontrol == beide){
        Serial.println("back");
        motora.downward();
        motorb.downward();
    }else if(motorcontrol == controla){
        motora.downward();
    }else if (motorcontrol == controlb){
        motorb.downward();
    }
}

void motorstop(){
    Serial.println("stop");
    motora.stop();
    motorb.stop();
}
#endif
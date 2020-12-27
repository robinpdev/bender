
encoder enc1{0, 22, 28, 30, false};
encoder enc2{0, 24, 26, 32, false};

int enc1_prevpos = 0;
int enc2_prevpos = 0;

void enc1_gotcha()
{
    if (digitalRead(enc1.cha) && digitalRead(enc1.chb0))
    {
        enc1.pos--;
    }
    else if (digitalRead(enc1.cha1) && !digitalRead(enc1.chb1))
    {
        enc1.pos++;
    }
}

void enc1_refreceived()
{
    enc1.gotref = true;
    attachInterrupt(digitalPinToInterrupt(enc1.cha), enc1_gotcha, RISING);
}

void enc1_start()
{
    pinMode(enc1.cha, INPUT);
    pinMode(enc1.chb, INPUT);
    pinMode(enc1.chr, INPUT);
    attachInterrupt(digitalPinToInterrupt(enc1.chr), enc1_refreceived, RISING);
}

void enc2_gotcha()
{
    if (digitalRead(enc2.cha) && digitalRead(enc2.chb0))
    {
        enc2.pos--;
    }
    else if (digitalRead(enc2.cha1) && !digitalRead(enc2.chb1))
    {
        enc2.pos++;
    }
}

void enc2_refreceived()
{
    enc2.gotref = true;
    attachInterrupt(digitalPinToInterrupt(enc2.cha), enc2_gotcha, RISING);
}

void enc2_start(void refreceived(), void mgotcha(), void mgotchb())
{
    pinMode(enc2.cha, INPUT);
    pinMode(enc2.chb, INPUT);
    pinMode(enc2.chr, INPUT);
    attachInterrupt(digitalPinToInterrupt(enc2.chr), enc2.refreceived, RISING);
}

struct encoder
{
public:
    volatile int pos = 0;
    const short cha, chb, chr;
    volatile bool gotref = false;
};
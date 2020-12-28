class encoder
{
public:
    volatile int pos = 0;
    volatile int prevpos = 0;
    short cha, chb, chr;
    volatile bool gotref = false;

    encoder(short mcha, short mchb, short mchr)
    {
        cha = mcha;
        chb = mchb;
        chr = mchr;
    }
};

encoder enca(22, 28, 30);
encoder encb(24, 26, 32);

void enc1_gotcha()
{
    if (digitalRead(enca.cha) && digitalRead(enca.chb))
    {
        enca.pos--;
    }
    else if (digitalRead(enca.cha) && !digitalRead(enca.chb))
    {
        enca.pos++;
    }
}

void enca_refreceived()
{
    enca.gotref = true;
    attachInterrupt(digitalPinToInterrupt(enca.cha), enc1_gotcha, RISING);
    detachInterrupt(digitalPinToInterrupt(enca.chr));
}

void enca_start()
{
    Serial.println("started enc1");
    pinMode(enca.cha, INPUT);
    pinMode(enca.chb, INPUT);
    pinMode(enca.chr, INPUT);
    attachInterrupt(digitalPinToInterrupt(enca.chr), enca_refreceived, RISING);
}

void encb_gotcha()
{
    if (digitalRead(encb.cha) && digitalRead(encb.chb))
    {
        encb.pos--;
    }
    else if (digitalRead(encb.cha) && !digitalRead(encb.chb))
    {
        encb.pos++;
    }
}

void encb_refreceived()
{
    encb.gotref = true;
    attachInterrupt(digitalPinToInterrupt(encb.cha), encb_gotcha, RISING);
    detachInterrupt(digitalPinToInterrupt(encb.chr));
}

void encb_start()
{
    Serial.println("started enc2");
    pinMode(encb.cha, INPUT);
    pinMode(encb.chb, INPUT);
    pinMode(encb.chr, INPUT);
    attachInterrupt(digitalPinToInterrupt(encb.chr), encb_refreceived, RISING);
}
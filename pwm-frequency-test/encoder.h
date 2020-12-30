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

encoder rotenc(50, 51, 30); //ref is niet verbonden


void rotenc_gotcha()
{
    if (digitalRead(rotenc.cha) && digitalRead(rotenc.chb))
    {
        rotenc.pos--;
    }
    else if (digitalRead(rotenc.cha) && !digitalRead(rotenc.chb))
    {
        rotenc.pos++;
    }
}

void rotenc_refreceived()
{
    rotenc.gotref = true;
    attachInterrupt(digitalPinToInterrupt(rotenc.cha), rotenc_gotcha, RISING);
    detachInterrupt(digitalPinToInterrupt(rotenc.chr));
}

void rotenc_start()
{
    Serial.println("started enc1");
    pinMode(rotenc.cha, INPUT);
    pinMode(rotenc.chb, INPUT);
    pinMode(rotenc.chr, INPUT);
    attachInterrupt(digitalPinToInterrupt(rotenc.chr), rotenc_refreceived, RISING);
}


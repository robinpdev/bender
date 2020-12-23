//Defines so the device can do a self reset
#define SYSRESETREQ    (1<<2)
#define VECTKEY        (0x05fa0000UL)
#define VECTKEY_MASK   (0x0000ffffUL)
#define AIRCR          (*(uint32_t*)0xe000ed0cUL) // fixed arch-defined address
#define REQUEST_EXTERNAL_RESET (AIRCR=(AIRCR&VECTKEY_MASK)|VECTKEY|SYSRESETREQ)

String returned = "";

volatile const byte cha1 = 22; // pin voor kanaal-A van encoder 1.
volatile const byte chb1 = 28; // pin voor kanaal-B van encoder 1.
volatile const byte chr1 = 30; // pin voor refferentiekanaal van encoder 1.

volatile const byte cha2 = 24;
volatile const byte chb2 = 26;
volatile const byte chr2 = 32;

bool eainit = false, ebinit = false;

volatile int vorigwaarde1 = 0;
volatile int totalpuls1 = 0; // variable dat het aantal getelde pulsen opslaat, LET OP: hier worden ook pulsen af van getrokken.
volatile bool geenpuls1 = true; // variable dat opslaat als de arduino tijd heeft om code uit te voeren dat niet met de encoder te maken heeft (die dus buiten de interrupt-routine valt).

volatile int vorigwaarde2 = 0;
volatile int totalpuls2 = 0; // variable dat het aantal getelde pulsen opslaat, LET OP: hier worden ook pulsen af van getrokken.
volatile bool geenpuls2 = true;

volatile long prevmili;

volatile int refstatus = 1; //vorig nul, ongetest

int maup = 9;
int madown = 8;

int mbup = 12;
int mbdown = 11;

int mspeed, maxcomps;

//          motor compensation variables            //
int master = 1;     //master-motor, 1 of 2
int mspeedi = 70;   //snelheid in % van master
int minspeedi = 25; //minimumsnelheid in % van master bij compensatie
int maxcompsi = 30; //maximum snelheidverschil slave
int mindelta = 2;   //minimum afwijking pulsen bij begin compensatie
int comprange = 10; //pulsen delta tot max snelheidsverschil slave
bool comp = false;  //status compensatie --AFBLIJVEN--

//pins voor knoppen
const int bup = 7;
const int bdown = 6;
boolean bcon = false; //master status knopcontrole
//status controle aparte knoppen
boolean bupon = true;
boolean bdownon = true;
boolean econtrol = true; //check als de 2 encoders geinitialiseerd zijn

const int buzpin = 4; //pin buzzer

String dir = "off";

int stage = 0;           // stage van het programma, zie txt

const int pmled = 2; //pin voor plooimodus-indicator

// doelbepaling variabelen in pulsen
int blokhoogte   = 90 / 0.02;  // blokhoogte
int target    = -1;         // doel -- default leeg
int meshoogte = 5 / 0.02;
int dikte     = 0;
int diepte    = 0;
int aanslag   = 0;

void setup() { // dit stuk code wordt eens uitgevoerd wanneer de arduino is opgestart

  stage = 1;
  
  mspeed = map(mspeedi, 0, 100, 0, 255);
  maxcomps = map(maxcompsi, 0, 100, 0, 255);
  
  pinMode(cha1, INPUT); // deze pinMode lijnen declareren de pins van de encoder-kanalen als invoerpins.
  pinMode(chb1, INPUT);
  pinMode(chr1, INPUT);

  pinMode(cha2, INPUT); // deze pinMode lijnen declareren de pins van de encoder-kanalen als invoerpins.
  pinMode(chb2, INPUT);
  pinMode(chr2, INPUT);

  pinMode(maup, OUTPUT);
  pinMode(madown, OUTPUT);

  pinMode(mbup, OUTPUT);
  pinMode(mbdown, OUTPUT);

  pinMode(bup, INPUT);
  pinMode(bdown, INPUT);

  pinMode(pmled, OUTPUT);

  //analogWrite(11, 100);

  Serial.begin(115200); // start de seriële communicatie op een snelheid van 115200 bits per seconde.
  Serial3.begin(115200);
  for(int i = 13; i < 10; i++){
    pinMode(i, OUTPUT);   
  }

  attachInterrupt(chr1, beginpuls, RISING);
  attachInterrupt(chr2, beginpuls2, RISING);
  //attachInterrupt(cha1, inpuls, RISING); // start de interrupt-routine op de pin van kanaal-A van encoder 1 en start de functie inpulsA wanneer de voltage omhooggaat.
  bcon = true;

  stage = 2;
  beep();
  if(bcon){ stage = 3; }

  sendchar('s', '3');
  sendint('b', (int)blokhoogte * 0.02);
  sendint('m', (int)meshoogte * 0.02);
  sendint('u', (int)mspeedi);
  sendint('l', (int)minspeedi);

}

void loop() {
  
  if(geenpuls1 && geenpuls2 && millis() >= prevmili + 100){ // dit stuk code wordt uitgevoerd als er geen puls 
    mcompensate();

    if(bcon){handlebuttons();} //als er knopcontrole is, behandel deze input dan

    if(stage == 4){
      if(totalpuls1 >=5000){
        if(target > 0){
          stage = 6;
          bcon = true;
          bupon = true;
          bdownon = true;
        }else{
          stage = 5;
          Serial.println("stage = 5");
          sendchar('s', '5');
          beep();
          bdownon = true;
          digitalWrite(pmled, HIGH);
        }
      }
    }

    
    
    if(vorigwaarde1 != totalpuls1){

      //Serial.println('p');
      prevmili = millis();
      updateverhouding();
      vorigwaarde1 = totalpuls1;
      //Serial.println("sending 1");
      sendint('q', totalpuls1);
      //Serial.println("moved q");

    }

    if(vorigwaarde2 != totalpuls2){

      //Serial.println("sending 2");
      sendint('p', totalpuls2);
      //Serial.println('q');
      prevmili = millis();
      vorigwaarde2 = totalpuls2; 
      //Serial.println("moved p");

      if(stage == 6){
        int deltad = totalpuls1 - target;
        Serial.print("verschil is ");
        Serial.println(deltad);
        if(deltad <= 400 && deltad >= 0){
          int mspeed2 = map(deltad, 0, 400, minspeedi, mspeedi);
          mspeed = map(mspeed2, 0, 100, 0, 255);
          movemotor(dir);
          bdownon = true;
        }
        else if(deltad <= 0){
          Serial.println("turning off");
          if(dir == "down"){ movemotor("off"); }
          bdownon = false;
        }
        else{
          mspeed = map(mspeedi, 0, 100, 0, 255);
          bdownon = true;
          movemotor(dir);
        }
      }
    }

  }

  if(geenpuls1 || geenpuls2){

    updatepackets();
    
  }

}

int bstate = -1;
void handlebuttons(){
  if(digitalRead(bup) && bstate != 1 && bupon){
    Serial.println("bup");
    digitalWrite(maup, HIGH);
    digitalWrite(mbup, HIGH);
    digitalWrite(madown, LOW);
    digitalWrite(mbdown, LOW);
    bstate = 1;

    if(comp){movemotor("up");}
  }
  if(digitalRead(bdown) && bstate != 2 && bdownon){
    Serial.println("bdown");
    digitalWrite(madown, HIGH);
    digitalWrite(mbdown, HIGH);
    digitalWrite(maup, LOW);
    digitalWrite(mbup, LOW);
    bstate = 2;
    if(comp){movemotor("down"); }
  }
  if(!digitalRead(bup) && !digitalRead(bdown) && bstate != 0){
    Serial.println("boff");
    digitalWrite(madown, LOW);
    digitalWrite(mbdown, LOW);
    digitalWrite(maup, LOW);
    digitalWrite(mbup, LOW);
    bstate = 0;
    movemotor("off"); //zeer belangrijk
  }

  if(eainit && ebinit && econtrol){
    Serial.println("button shutdown");
    //bcon = false;
    bdownon = false;
    econtrol = false;
    digitalWrite(madown, LOW);
    digitalWrite(mbdown, LOW);
    digitalWrite(maup, LOW);
    digitalWrite(mbup, LOW);
    comp = true;
    
    beep();
    stage = 4;
    sendchar('s', '4');
    
    
  }
  
}

void movemotor(String mdir){
  dir = mdir;
  if(dir != "up" && dir != "down" && dir != "update"){
    digitalWrite(maup, LOW);
    digitalWrite(madown, LOW);
    digitalWrite(mbup, LOW);
    digitalWrite(mbdown, LOW);
  }
  else if(dir == "up"){
    analogWrite(maup, mspeed);
    mcompensate();
  }
  else if(dir == "down"){
    analogWrite(madown, mspeed);
    mcompensate();
  }

}

void beep(){
  analogWrite(buzpin, 120); 
  delay(100);
  digitalWrite(buzpin, LOW);
}

void beep(int duration){
  analogWrite(buzpin, 120);
  delay(duration);
  digitalWrite(buzpin, LOW);
}

void beep(int duration, int freq){
  analogWrite(buzpin, freq);
  delay(duration);
  digitalWrite(buzpin, LOW);
}
  

void inpuls(){ // dit stuk code wordt uitgevoerd wanneer er een positieve flank wordt ontvangen op cha1.
 
  geenpuls1 = false;

  if (digitalRead(cha1) && digitalRead(chb1)){ // deze code wordt uitgevoerd wanneer er bij cha1 en chb1 een positieve waarde wordt gelezen.
    
    totalpuls1 --; // telt 1 op bij het variable totalpuls1;
    //Serial.print('+');
    
  }

  else if (digitalRead(cha1) && !digitalRead(chb1)) {  // deze code wordt uitgevoerd wanneer er een positieve flank wordt gemeten bij cha1 en bij chb1 een low waarde wordt gemeten.
       
    totalpuls1 ++; // trekt 1 af van het variable totalpuls1;
    //Serial.print('-');
     
  }

  geenpuls1 = true; // er wordt geen puls meer ontvangen dus is er tijd om waarde door te zenden.
}



void inpuls2(){ // dit stuk code wordt uitgevoerd wanneer er een positieve flank wordt ontvangen op cha1.
  
  geenpuls2 = false;
  
  
  if (digitalRead(cha2) && digitalRead(chb2)){ // deze code wordt uitgevoerd wanneer er bij cha1 en chb1 een positieve waarde wordt gelezen.
    
    totalpuls2 --; // telt 1 op bij het variable totalpuls1;
    //Serial.print('+');
    
  }

  else if (digitalRead(cha2) && !digitalRead(chb2)) {  // deze code wordt uitgevoerd wanneer er een positieve flank wordt gemeten bij cha1 en bij chb1 een low waarde wordt gemeten.
       
    totalpuls2 ++; // trekt 1 af van het variable totalpuls1;
    //Serial.print('-');
     
  }

  geenpuls2 = true; // er wordt geen puls meer ontvangen dus is er tijd om waarde door te zenden.
}




void beginpuls(){
  
  attachInterrupt(cha1, inpuls, RISING);
  Serial.println("start 1");
  eainit = true;// start de interrupt-routine op de pin van kanaal-A van encoder 1 en start de functie inpulsA wanneer de voltage omhooggaat.
  totalpuls1 = 4250;
  detachInterrupt(digitalPinToInterrupt(chr1));
}

void beginpuls2(){  
  attachInterrupt(cha2, inpuls2, RISING);
  //detachInterrupt(digitalPinToInterrupt(chr2));
  Serial.println("start 2");
  ebinit = true;
  totalpuls2 = 4250;
  detachInterrupt(digitalPinToInterrupt(chr2));
}

void sendchar(char var, char input) {
  Serial3.write('!');
  Serial3.write(var);
  Serial3.write(input);
  Serial3.write('#');
  //npackets ++;
  //nrpackets ++;
}

void sendstring(char var, String input) {
  const int len = input.length() + 1;
  Serial3.write('!');
  Serial3.write(var);
  char buf[len];
  input.toCharArray(buf, len);
  for (int n = 0; n <= len - 1; n++) {
    Serial3.write(buf[n]);
  }
  Serial3.write('#');
  //npackets ++;
  //nrpackets ++;
}

void sendint(char var, int input) {
  //String conversion = String(input);
  //const int len = conversion.length();
  Serial3.write('!');
  Serial3.write(var);
  /*char buf[len + 1];
  conversion.toCharArray(buf, len + 1);
  for (int n = 0; n <= len - 1; n++) {
    Serial3.print(buf[n]);
    Serial.print(buf[n]);
  }
  Serial.println("");*/
  Serial3.print(String(input));
  Serial3.write('#');
  //npackets ++;
  //nrpackets ++;
}

void sendfloat(char var, float input) {
  Serial3.write('!');
  Serial3.write(var);
  Serial3.print(input);
  Serial3.write('#');
  //npackets ++;
  //nrpackets ++;
}

int prevarrow = 0;
void updateverhouding(){}

void alarm(String message){
  sendstring('a', message);
  beep(700);
  reset_board();
}


void updatepackets(){
  
  while(Serial3.available() >= 3 && Serial3.read() == '!'){
    returned = "";
    int stat = 1;
    bool on = true;
    char action = Serial3.read();
    Serial.println(action);
    char data;
    
    while(on == true && Serial3.available()){     
      
      data = Serial3.read();
      if(data == '#' && returned != ""){
        
        on = false;
        
        if(stat == 1){
          Serial3.write('R');
        }
        
        else if (stat == 2){
          Serial3.write('W');
        }
        
      }
      
      else{
        returned += data;
      }
      
      if(!Serial3.available() && stat == 1 && on == true){
        stat = 2;
        delay(20);///////////////////////////////////experimenteeeel
      }
    
    }

    /*if(action == 'o'){
      if(action = 'a'){
        totalpuls1 = 0;
        totalpuls2 = 0;
      }
    }*/

    if(action == 'a'){
      comp = false;
        if(returned == "i"){
          Serial.println("got i");
          digitalWrite(maup, HIGH);
        }
        if(returned == "o"){
          Serial.println("got o");
          digitalWrite(maup, LOW);
          digitalWrite(madown, LOW);
        }
        
    }

    if(action == 'z'){
      comp = false;
        if(returned == "i"){
          Serial.println("got i");
          digitalWrite(madown, HIGH);
        }
        
    }

    if(action == 'b'){
      comp = false;
        if(returned == "i"){
          Serial.println("got i");
          digitalWrite(mbup, HIGH);
        }
        if(returned == "o"){
          Serial.println("got o");
          digitalWrite(mbup, LOW);
          digitalWrite(mbdown, LOW);
        }
        
    }
    if(action == 'y'){
      comp = false;
        if(returned == "i"){
          Serial.println("got i");
          digitalWrite(mbdown, HIGH);
        }
    }

    if(action == 't'){

        if(returned == "i"){
          Serial.println("got i");

          if(eainit && ebinit){
            Serial.println("starting compensation!");
            analogWrite(maup, mspeed);
            comp = true;
            dir = "up";
            mcompensate();
          }else{
            comp = false;
            digitalWrite(maup, HIGH);
            digitalWrite(mbup, HIGH);
          }
          
        }
        if(returned == "o"){
          comp = false;
          Serial.println("got o");
          digitalWrite(maup, LOW);
          digitalWrite(madown, LOW);
          digitalWrite(mbup, LOW);
          digitalWrite(mbdown, LOW);
        }
        
    }
    if(action == 'u'){
      comp = false;
        if(returned == "i"){
          Serial.println("got i");
          digitalWrite(madown, HIGH);
          digitalWrite(mbdown, HIGH);
        }
    }

    if(action == 'd'){ //doel van mega komt binnen
      
      float btarget = (returned.toInt() / 100) / 0.02;
      target = (int) btarget;
      Serial.print("target is");
      Serial.println(target);
      if(stage == 5){
        stage = 6;
        sendchar('s', '6');
        Serial.println("got goal");
        bcon = true;
        bupon = true;
        bdownon = true;
      }

    }
    
    if(action == 'f'){ reset_board(); }
    
    if(action == 'g'){
      if(mspeedi > 100){ alarm("procent boven 100"); }
      else{
        mspeedi = returned.toInt() / 100;
        mspeed = map(mspeedi, 0, 100, 0, 255);
        beep();
      }
    }
    if(action == 'l'){
      if(minspeedi > 100 || minspeedi > mspeedi){ alarm("procent boven maxspd of 100"); }
      else{
        minspeedi = returned.toInt() / 100;
        beep();
      }
    }

    if(action == 'q' || action == 'r' || action == 'i' || action == 'h' || action == 'm'){
      if(action == 'q'){ dikte = returned.toInt() / 100 / 0.02; Serial.println("got dikte"); }
      if(action == 'r'){ diepte = returned.toInt() / 100 / 0.02; Serial.println("got diepte"); }
      if(action == 'i'){ aanslag = returned.toInt() / 100 / 0.02; Serial.println("got aanslag"); }
      if(action == 'h'){ blokhoogte = returned.toInt() / 100 / 0.02; Serial.println("got blokhoogte"); }
      if(action == 'm'){ meshoogte = returned.toInt() / 100 / 0.02; Serial.println("got meshoogte"); }
      
      target = blokhoogte - diepte + dikte + meshoogte;
      beep();

      if(stage == 5){
        stage = 6;
        sendchar('s', '6');
        Serial.println("started stage 6");
        bcon = true;
        bupon = true;
        bdownon = true;
      }

      if(blokhoogte + meshoogte > 170 / 0.02){
        alarm("doel te hoog");
      }
      
      Serial.print("doel is nu ");
      Serial.print(target);
      Serial.println(" pulsen");
      
      Serial.print("doel is nu ");
      Serial.print(target * 0.02);
      Serial.println(" mm");
    }

  }

}

void reset_board() // Restarts program from beginning but does not reset the peripherals and registers
{
  REQUEST_EXTERNAL_RESET;
} 

void mcompensate(){
  
  if(comp && eainit && ebinit){
    if(dir == "up"){
      digitalWrite(mbdown, LOW);
      int deltap = totalpuls2 - totalpuls1;
      if((deltap > mindelta && deltap > 0)){
        int deltas = map(deltap, mindelta, mindelta + comprange, 0, maxcomps);
        if(deltas > maxcomps){ deltas = maxcomps; }
        analogWrite(mbup, mspeed - deltas);
        Serial.print("compensating up with ");
        Serial.println(map(-deltas, 0, 255, 0, 100));
      }else if(deltap < -mindelta && deltap < 0){
        int deltas = map(-deltap, mindelta, mindelta + comprange, 0, maxcomps);
        if(deltas > maxcomps){ deltas = maxcomps; }
        analogWrite(mbup, mspeed + deltas);
        Serial.print("compensating up with ");
        Serial.println(map(deltas, 0, 255, 0, 100));
      }
    }

    if(dir == "down"){
      digitalWrite(mbup, LOW);
      int deltap = totalpuls2 - totalpuls1;
      if((deltap > mindelta && deltap > 0)){
        int deltas = map(deltap, mindelta, mindelta + comprange, 0, maxcomps);
        if(deltas > maxcomps){ deltas = maxcomps; }
        analogWrite(mbdown, mspeed + deltas);
        Serial.print("compensating up with ");
        Serial.println(map(-deltas, 0, 255, 0, 100));
      }else if(deltap < -mindelta && deltap < 0){
        int deltas = map(-deltap, mindelta, mindelta + comprange, 0, maxcomps);
        if(deltas > maxcomps){ deltas = maxcomps; }
        analogWrite(mbdown, mspeed - deltas);
        Serial.print("compensating up with ");
        Serial.println(map(deltas, 0, 255, 0, 100));
      }
    }
  }
  
}





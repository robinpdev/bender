//setup van alle librarys.
//#include <cstddef>
#include <Keypad.h> //library van keypad
#include <TFT_HX8357.h> // Hardware-specific library
#include <Adafruit_GFX.h>    // Core graphics 
#include <Wire.h> //i2c library
TFT_HX8357 tft = TFT_HX8357();
//definiëring van kleuren.

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    int(210/8*2048) + int(216/4*32)+ int(224/8)
#define ORANGE    int(237/8*2048) + int(163/4*32)+ int(35/8)


const byte ROWS = 4; //vier rijen
const byte COLS = 4; //vier kolommen

char hexaKeys[ROWS][COLS] = {
  {'D','C','B','A'},
  {'#','9','6','3'},
  {'0','8','5','2'},
  {'*','7','4','1'}
};

//definiering van de pins waarop de keypad is aangesloten
byte rowPins[ROWS] = {A0, A1, A2, A3}; 
byte colPins[COLS] = {A4, A5, A6, A7}; 

//maak object keypad
Keypad keypad1 = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

int background = WHITE;

String lastmessage = "";


class rect{
  public:

    int x, y, width, height;

    rect(int mx, int my, int mwidth, int mheight){
      x = mx;
      y = my;
      width = mwidth;
      height = mheight;
    }

    void draw(int color){
      tft.fillRect(x, y, width, height, color);
    }
};


class item{

  public:

    int margintop;
    String text;
    bool isinput;
    int textsize;
    bool selected;
    int endpos;
    int drawnx;
    int drawny;
    int writex;
    String msg = "";
    String btext = "";
    int color;

    item(int mmargintop, String mtext, int mtextsize, bool misinput){
    
      margintop = mmargintop;
      text = mtext;
      isinput = misinput;
      textsize = mtextsize;
      writex = (int) text.length() * 6 * textsize;

    }

    item(int mmargintop, String mtext, String mbtext, int mtextsize, bool misinput){
    
      margintop = mmargintop;
      text = mtext;
      isinput = misinput;
      textsize = mtextsize;
      btext = mbtext;
      writex = (int) text.length() * 5.5 * textsize;

    }

    void draw(int x, int y, bool margin){
      
      drawnx = x;
      drawny = y;
      
      tft.setTextColor(BLUE);
      tft.setTextSize(textsize);
      tft.setTextWrap(false);
      if(margin){tft.setCursor(x, y + margintop); endpos = y + textsize * 7 + margintop; drawny += margintop;}
      else{tft.setCursor(x, y); endpos = y + textsize * 7;}
      tft.print(text);

      if(msg != ""){ update(msg, RED); }
      
    }

    void update(String mmsg, int mcolor){
      
      msg = mmsg;
      color = mcolor;
      
      tft.fillRect(drawnx + writex, drawny, 700, textsize * 7, background);
      
      tft.setTextColor(color);
      tft.setTextSize(textsize);
      tft.setTextWrap(false);
      tft.setCursor(drawnx + writex, drawny);
      tft.print(msg + btext);
      
    }


    char input (){

      select();
      
      String omsg = msg;
      
      char input = keypad1.getKey(); //initialisatie van inputs naar een niet bestaande toets
      char prevkey = keypad1.getKey();
      bool done = false;
      float output = -1; //output initialiseren
      int curs = 1; //hulpvariable voor komma
      String conversion; //initialiseer string voor conversie string naar int
      while(done == false){ //wanneer de input niet bevestigd is
        delay(10);
        input = keypad1.getKey();

        if(input != prevkey && prevkey != 'x'){
          Serial.print("key ("); 
          Serial.print(prevkey); 
          Serial.println(") released");
        }
  
        if(input !=' x' && input != prevkey){ 
          Serial.print("key ("); 
          Serial.print(input); 
          Serial.println(") pressed");

          if(isDigit(input)){//als ingegeven karakter een nummer is
            conversion = input;
            if(output == -1){ output = 0;}
            //Serial.println((float)conversion.toInt() / curs); //geef waarde tot nu toe weer op seriele monitor
      
            if(curs >= 10){ //voegt cijfers bij in een correcte manier.
              //als er komma is ingegeven
              output = output + (float)conversion.toInt() / curs; 
              curs = curs * 10;        
            }
            else{
              output = (output * 10) + (float)conversion.toInt();
            }
            //teken doel op scherm
            update(String(output), RED);
          
          }

          else if(input == '*'){ //wanneer toets komma wordt ingedrukt
            curs = curs * 10;
          }
    
          else if(input == 'D'){//wanneer er wordt op de toets bevestigen gedrukt
            
          
            if(output != -1){     
              //Serial.println("returning v");   
              //Serial.println(output);
              deselect();
              return 'v';
            }
            else{
              update(omsg, RED);
              deselect();
              //Serial.println("returning e");
              return 'e';
            }    
          }
          else if(input == '#'){
            done = true; //stop while loop voor selecteren
            update(omsg, RED);
            deselect();
            //Serial.println("returning #");
            return '#';
          }
        
        } 
  
        if(input != prevkey){
          prevkey = input;
        }
        prevkey = input;
        
        if(input != ' '){
          //Serial.println(input);
        }
    
      }
      
    }

    void select(){

      tft.drawRect(drawnx - 4, drawny - 4, 480 - drawnx - 8, textsize * 10 + 8, RED);
      
    }

    void deselect(){

      tft.drawRect(drawnx - 4, drawny - 4, 480 - drawnx - 8, textsize * 10 + 8, background);
      
    }

    /*char getkey(){

      for(int col = 0; col <= 3; col ++){

      digitalWrite(colPins[col], HIGH);

      delay(10);

      bool got = false;
    
      for(int row = 0; row <= 3; row ++){
        int val = analogRead(rowPins[row]);
        delay(5);
        if(val >= 1000){
          digitalWrite(colPins[col], LOW);
          return hexaKeys[row][col];
          got = true;
          //break;
        }
      }
      digitalWrite(colPins[col], LOW);

    }

    return 'x';
 
  }*/

    

};



class screen{

  public:

    item *items;
    int icursor;
    int lastsel = 0;
    int nitems;
    
    screen(item mitems[], int mnitems){

      items = mitems;
      nitems = mnitems;
      //Serial.println(sizeof(items));
    }

    void draw(int x, int y){

      tft.fillRect(0, 0, 480, 270, background);
      
      //Serial.println(sizeof(items));
      for(int i = 0; i < nitems; i++){
        //Serial.println(i);
        if(i == 0){ items[i].draw(x, y, false); }
        else{ items[i].draw(x, items[i - 1].endpos, true); }
      }
    }

    item get(String query){

      for(int i = 0; i <= nitems; i++){

        if(items[i].text == query ){

          //Serial.println("query of (" + query + ") succesfull");
          return items[i];
          
        }
        
      }

      Serial.println("query for " + query + " failed");
      return items[0];

    }

    

    void setmsg(String query, int val){
      
      for(int i = 0; i <= nitems; i++){

        if(items[i].text == query ){

          items[i].msg = String(val);
          
        }
        
      }
    }

    void iselect(int item){

      items[item].select();
      
    }

    void ideselect(int item){

      items[item].deselect();
      
    }

    char select(){
      
        int sstat = 0;
        char outcome = 'n';
        while(sstat == 0){
          if(items[icursor].isinput){ 
            //Serial.println(icursor);
            sstat = 1;
            outcome = items[icursor].input();
            lastsel = icursor;
            icursor ++;
            if(icursor >= nitems || outcome == '#'){ sstat = 2; icursor = 0; }
            return outcome; 
          }
          if(icursor >= nitems - 1){ sstat = 2; icursor = 0; return 'n'; }
          if(sstat == 0){icursor ++; }
        }
        //Serial.println(icursor);


        icursor ++;
        if(icursor >= nitems || outcome == '#'){ icursor = 0;}

        return outcome;
      
    }
    

    
};





  item pos = item(10, "pos: ", 5, false);
  //item delta = item(10, "delta: ", 3, false);
  item dikte = item(10, "dikte:", 3, true);
  item diepte = item(10, "diepte:", 3, true);
  item aanslag = item(10, "aanslag: ", 3, false);

  item t1items[] = {pos, dikte, diepte, aanslag};
  screen tab1 = screen(t1items, 4);


  item hoogte = item(10, "hoogte: ", 3, true);
  item mes = item(10, "mes: ", 3, true);
  item imaxspd = item(10, "maxspd: ", 3, true);
  item iminspd = item(10, "minspd: ", 3, true);
  item t2items[] = {hoogte, mes, imaxspd, iminspd};
  screen tab2 = screen(t2items, 4);

  item doel = item(20, "doel:", "mm", 3, false);
  item delta = item(20, "delta: ", "mm", 3, false);
  item t3items[] = {doel, delta};
  screen tab3 = screen(t3items, 2);
  
  screen tabs[] = {tab1, tab2, tab3};
  int ctab = 0;

  rect tb1 = rect(40, 275, 120, 40);
  rect tb2 = rect(180, 275, 120, 40);
  rect tb3 = rect(320, 275, 120, 40);

  rect tbs[] = {tb1, tb2, tb3};

  int pos1;
  int pos2;
  int pos_ans = 0;

  int afst_pb;
  int pdikte;
  int pdiepte;
  int ddoel = -1;
  int nstatus = 0;
  int blokhoogte = -1, meshoogte = -1, maxspd = -1, minspd = -1;

  String statusmsg;

  bool received;

  int currentcontrol = 0;

  bool sentmoff = false;

void setup() {
  // start communicatie met pc en due
  Serial.begin(115200);
  Serial1.begin(115200);
  
  // start scherm
  tft.begin();
  tft.setRotation(5);
  tft.fillScreen(WHITE);

  // definieer modus pins van keypad
  for(int row = 0; row <= 3; row++){
    pinMode(colPins[row], OUTPUT);
    pinMode(rowPins[row], INPUT);
  }

  // teken tabblad
  tabs[ctab].draw(10, 10);
  //teken balken
  if(ctab == 2){
    tft.setTextSize(5);
    tft.setCursor(400, 10);
    tft.print("L");
    tft.setCursor(455, 10);
    tft.print("R");
    
    tft.drawRect(400, 50, 30, 150, RED);
    tft.drawRect(455, 50, 30, 150, RED);
  }

  // verander bericht van status 
  //tabs[ctab].get(":").update("zoek naar ref", RED);

  // teken tabknoppen
  tft.fillRect(0, 270, 480, 50, RED);
  tbs[ctab].draw(GREEN);
  tbs[1].draw(BLUE);
  tbs[2].draw(BLUE);
  drawtablabels();

  Serial.println("test");

  //klaar met initialisatie, begin routine voor het starten van de due
  handlemessage("opgestart, due opstarten");
  sendchar('f', 'f');
}

char prevkey = 'x'; //hulpvariabele keypad

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad1.getKey(); //lees toets van keypad

  if(key != prevkey && prevkey != ' '){ //wanneer een toets wordt losgelaten
    Serial.print("key ("); 
    Serial.print(prevkey); 
    Serial.println(") released");
    handlekeyup(prevkey);
  }
  
  if(key !=' ' && key != prevkey){ //wanneer een toets wordt ingedrukt
    Serial.print("key ("); 
    Serial.print(key); 
    Serial.println(") pressed");
    handlekey(key);
  } 
  
  if(key != prevkey){ //sla vorige gedrukte toets op
    prevkey = key;
  }
  
  updatepackets(); //lees paketten van due
  if(received){ updatescreen(); } //als data van due binnenkomt

  delay(10); //doe dit deel van de code 100 keer per seconde
  
}

String checkval(int input){
  if(input >= 0){
    return String(input);
  }else{
    return "niet ontvangen";
  }
}

void handlekey(char key){ // behandeling van ingedrukte toetsen
  
  if(key == 'D'){

    if(true){

      tabs[ctab].icursor = 0;
      handleinput( tabs[ctab].select(), tabs[ctab]);
      prevkey = 'x';

    }
    else{

      tabs[1].ideselect(currentcontrol);
      
      currentcontrol ++;
      if(currentcontrol >= 3){
        currentcontrol = 0;
      }
      else{
        tabs[1].iselect(currentcontrol);
      }
      
    }

  }
  
  if(key == 'A'){ //verandering van tabblad

    tbs[ctab].draw(BLUE);
    ctab ++;
    if(ctab >= 3){ ctab = 0; }
    tbs[ctab].draw(GREEN);

    drawtablabels();
    
    if(ctab == 2){
      tabs[ctab].draw(25, 10);
      tft.setTextSize(5);
      tft.setCursor(400, 10);
      tft.print("L");
      tft.setCursor(455, 10);
      tft.print("R");
    
      tft.drawRect(400, 50, 30, 150, RED);
      tft.drawRect(455, 50, 30, 150, RED);
      
    }
    else{
      tabs[ctab].draw(25, 10);
    }

    if(ctab == 0){
      handlemessage();
    }else if(ctab == 1){
      updatet1();
    }

    updatescreen();
  }

  if(key == 'B'){ //noodstop motors
    sendchar('a', 'o');
    sendchar('b', 'o');
    Serial.println("sent emergency motor shutdown");
  }
  
  /*if(true){ //motorcontrole met keypad
    if(key == '1'){
      Serial.println("motor a on");
      sendchar('a', 'i');
    }
    if(key == '4'){
      Serial.println("motor a on");
      sendchar('z', 'i');
    }

    if(key == '2'){
      Serial.println("motor b on");
      sendchar('b', 'i');
    }
    if(key == '5'){
      Serial.println("motor b on");
      sendchar('y', 'i');
    }

    if(key == '3'){
      Serial.println("motor b on");
      sendchar('t', 'i');
    }
    if(key == '6'){
      Serial.println("motor b on");
      sendchar('u', 'i');
    }

  }*/

}

void handlekeyup(char key){ //behandeling losgelaten toets

  /*if(true){ //motorcontrole keypad
    if(key == '1' || key == '4'){
      Serial.println("motor a off");
      sendchar('a', 'o');
    }

    if(key == '2' || key == '5'){
      Serial.println("motor b off");
      sendchar('b', 'o');
    }

    if(key == '3' || key == '6'){
      Serial.println("both motors off");
      sendchar('t', 'o');
    }
  }*/
  
}

void drawtablabels(){
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  
  tft.setCursor(65, 284);
  tft.print("MAIN");

  tft.setCursor(205, 284);
  tft.print("CONF");

  tft.setCursor(345, 284);
  tft.print("DIAG");
}

void updatet1(){
  if(ctab == 1){
    tabs[ctab].get("hoogte: ").update(checkval(blokhoogte), RED);
    tabs[ctab].get("mes: ").update(checkval(meshoogte), RED);
    tabs[ctab].get("maxspd: ").update(checkval(maxspd), RED);
    tabs[ctab].get("minspd: ").update(checkval(minspd), RED);
  }
}

void handleinput(char input, screen scherm){
    Serial.println(input);
    if(input == '#'){
      //Serial.println("cancelled");
      //scherm.items[scherm.icursor].deselect();
      updatet1();
      return;
    }

    if(input == 'e'){
      Serial.println("geen value, doorscrollen");
      updatet1();
      handleinput(scherm.select(), scherm);
      return;
    }

    if(input == 'v'){
      Serial.println("got vvvs");
      Serial.println("value input");
      String var = scherm.items[scherm.lastsel].text;
      Serial.print(var);
      float val = scherm.items[scherm.lastsel].msg.toFloat(); //INT????????????
      Serial.println(val);
      
      if(var == "afstand pb: "){
        afst_pb = val;
      }

      if(var == "plaatdikte: "){
        pdikte = val;
      }

      if(var == "plooidiepte: "){
        pdiepte = val;
      }
      if(var == "doel:"){
        Serial.println("sending goal with ");
        Serial.println(val);
        sendint('d', (int)val * 100.0);
      }
      else if(var == "dikte:"){
        sendfloat('q', val);
        Serial.println("sent dikte");
      }
      else if(var == "diepte:"){
        sendfloat('r', val);
        Serial.println("sent diepte");
      }
      else if(var == "aanslag:"){
        sendfloat('i', val);
        Serial.println("sent aanslag");
      }
      else if(var == "hoogte: "){ sendfloat('h', val); }
      else if(var == "mes: "){ sendfloat('m', val); }
      else if(var == "maxspd: " && val <= 100){ sendfloat('g', val); maxspd = val; }
      else if(var == "minspd: " && val <= 100){ sendfloat('l', val); minspd = val; }
      
      scherm.icursor = 0;
    }

    /*if(afst_pb > 0 /*&& pdikte > 0 && pdiepte > 0 && posl > 0 && posr > 0){
      /*ddoel = afst_pb - pdikte + pdiepte;
      tab1.setmsg("doel: ", ddoel);
      Serial.println(ddoel);
    }*/
  
}


char getkey(){

  for(int col = 0; col <= 3; col ++){

    digitalWrite(colPins[col], HIGH);
    delay(10);
    bool got = false;
    
    for(int row = 0; row <= 3; row ++){
      int val = analogRead(rowPins[row]);
      delay(1);
      if(val >= 800){
        digitalWrite(colPins[col], LOW);
        return hexaKeys[row][col];
        got = true;
        //break;
      }
    }
    digitalWrite(colPins[col], LOW);

  }

  return 'x';
 
}

void handlemessage(String message){
  lastmessage = message;
  handlemessage();
}

void handlemessage(){
  if(ctab == 0){
    tft.fillRect(20, 220, 480, 40, WHITE);
    if(lastmessage == "on"){
      tft.setCursor(20, 220);
      tft.setTextSize(2);
      tft.setTextColor(BLACK);
      tft.print("due opgestart");
    }else{
      tft.setCursor(10, 220);
      tft.setTextSize(2);
      tft.setTextColor(BLACK);
      tft.print(lastmessage);
    }
  }
}


void handlepackets(char action, String returned){ //behandeling paketten
  received = true; //signaal aan loop om scherm te updaten

  if(action == 'b'){ blokhoogte = returned.toInt(); updatet1();}
  if(action == 'm'){ meshoogte = returned.toInt(); updatet1();}
  if(action == 'u'){ maxspd = returned.toInt(); updatet1();}
  if(action == 'l'){ minspd = returned.toInt(); updatet1();}
  
  if(action == 'k'){ Serial.println("refpuls ontvangen"); }

  if(action == 'q'){
    if(nstatus != 1){
      nstatus = 1;
      statusmsg = "online";
      
      if(ctab == 0){ 
        tabs[0].get(":").update(statusmsg, RED); 
        //tabs[0].get(":").msg = statusmsg;
      }
      
    }
    //Serial.print("pos L: ");
    //Serial.println(returned);
    pos1 = returned.toInt();
  }

  if(action == 'p'){
    statusmsg = "online";
    
    //Serial.print("pos R: ");
    //Serial.println(returned);
    pos2 = returned.toInt();
  }

  if(action == 'd'){
    ddoel = int(returned.toInt() * 0.02);
  }

  if(action == 's'){ //programma stage update
    if(returned == "3"){
      handlemessage("due opgestart, zoek ref");
    }
    if(returned == "4"){
      handlemessage("ref gevonden, beweeg naar boven");
    }
    if(returned == "5"){
      handlemessage("geef doel in");
    }
    if(returned == "6"){
      handlemessage("klaar voor plooien");
    }
    
  }

  if(action == 'c'){
    pos_ans = returned.toInt();
    updatescreen();
  }

}


void updatescreen(){ //update scherm met waardes van due
  if(ctab == 0){
    //tabs[0].get("pos L: ").update(String(pos1), RED);
    //tabs[0].get("pos R: ").update(String(pos2), RED);

    //tabs[0].get("delta:").update(String(pos1 - pos2), RED);
    tabs[0].get("pos:").update(String(pos1 * 0.02) + "mm", BLUE);

    tabs[0].get("aanslag: ").update(String(pos_ans), RED);

    Serial.print("left: ");
    Serial.println(pos1);

    Serial.println();

    Serial.print("right: ");
    Serial.println(pos2);
    
    received = false;
  }

  else if(ctab == 2){
    tft.fillRect(401, 51, 28, 148, WHITE); //BALKEN
    tft.fillRect(456, 51, 28, 148, WHITE);

    int h1 = map(pos1, 2000, 9000, 0, 150);
    int h2 = map(pos2, 2000, 9000, 0, 150);
    h2 += 0;

    if(pos1 != 0){tft.fillRect(400, 50 + h1, 30, 10, RED); }
    if(pos2 != 0){tft.fillRect(455, 50 + h2, 30, 10, RED); }

    tabs[2].get("delta: ").update(String((pos1 - pos2) * 0.02), RED);
    if(ddoel > 0){
      tabs[2].get("doel: ").update(String(ddoel), RED);
    }
  }
  received = false;
  
}

//functies voor het zenden van data naar due
void sendchar(char var, char input) {
  Serial1.write('!');
  Serial1.write(var);
  Serial1.write(input);
  Serial1.write('#');
}

void sendstring(char var, String input) {
  const int len = input.length() + 1;
  Serial1.write('!');
  Serial1.write(var);
  char buf[len];
  input.toCharArray(buf, len);
  for (int n = 0; n <= len - 1; n++) {
    Serial1.write(buf[n]);
  }
  Serial1.write('#');
}

void sendint(char var, int input) {
  String conversion = String(input);
  const int len = conversion.length();
  Serial1.write('!');
  Serial1.write(var);
  char buf[len + 1];
  conversion.toCharArray(buf, len + 1);
  for (int n = 0; n <= len - 1; n++) {
    Serial1.write(buf[n]);
  }
  Serial1.write('#');
}

void sendfloat(char var, float input) {
  /*Serial1.write('!');
  Serial1.write(var);
  Serial1.print(input * 100);
  Serial1.write('#');*/
  float between = input * 100;
  int sendi = (int)between;
  sendint(var, sendi);
  
}


void updatepackets(){ //functie voor het ontvangen van data van due
  while(Serial1.available() >= 3 && Serial1.read() == '!'){
    String returned = "";
    int stat = 1;
    bool on = true;
    char action = Serial1.read();
    Serial.println(action);
    char data;
    
    while(on == true && Serial1.available()){     
      
      data = Serial1.read();
      //Serial.print(data);
      if(data == '#' && returned != ""){
        
        on = false;
        
        if(stat == 1){
          //Serial1.write('R');
        }
        
        else if (stat == 2){
          //Serial1.write('W');
        }
        
      }
      
      else{
        returned += data;
      }
      
      if(!Serial1.available() && stat == 1 && on == true){
        stat = 2;
        Serial.println("fragmented packet, waiting");
        delay(10);
      }
      
    }

    handlepackets(action, returned);
    Serial1.flush(); //experimentele functie, voor herziening
   
  }

}

#include "graphics.h"
#include "keypad.h"

#define tarr(...) __VA_ARGS__ 

const short bgcolor = WHITE;
const short inputboxcolor = GREEN;
const short inputselcolor = RED;

#include "gui.h"

//****************TAB1**********************//
label posl("pos: ");
label posd("");
label diktel("dikte: ");
input diktei;
label dieptel("diepte: ");
input dieptei;
label aanslagl("aanslag: ");
input aanslagi;

label statusl("status");

nitems(icol1){&posl, &posd};
newcol(col1, icol1);

nitems(icol2){&diktel, &diktei};
newcol(col2, icol2);

nitems(icol3){&dieptel, &dieptei};
newcol(col3, icol3);

nitems(icol4){&aanslagl, &aanslagi};
newcol(col4, icol4);

nitems(items){&col1, &col2, &col3, &col4};
newgui(guim1, items);

//****************TAB2**********************//
label hoogtel("hoogte: ");
input hoogtei;
label mesl("mes: ");
input mesi;
label maxspdl("maxspd: ");
input maxspdi;
label minspdl("minspd: ");
input minspdi;

nitems(icol21){&hoogtel, &hoogtei};
newcol(col21, icol21);

nitems(icol22){&mesl, &mesi};
newcol(col22, icol22);

nitems(icol23){&maxspdl, &maxspdi};
newcol(col23, icol23);

nitems(icol24){&minspdl, &minspdi};
newcol(col24, icol24);

nitems(items2){&col21, &col22, &col23, &col24};
newgui(guim2, items2);

//****************TAB3**********************//
label doell("doel: ");
input doeli;
label deltal("delta: ");
input deltai;

nitems(icol31){&doell, &doeli};
newcol(col31, icol31);

nitems(icol32){&deltal, &deltai};
newcol(col32, icol32);

nitems(items3){&col31, &col32};
newgui(guim3, items3);

gui* guis[] = {&guim1, &guim2, &guim3};
byte tabindex = 1;
#define cgui guis[tabindex - 1] //current tab's gui

int pos1 = 3000, pos2 = 4000;

void setup()
{
  // start communicatie met pc en due
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("startup");

  // start scherm
  tft.begin();
  tft.setRotation(5);
  tft.fillScreen(WHITE);

  drawtab();
}

void loop()
{
  char readkey = keypad.getKey();
  if (readkey == 'D')
  {
    if (inputmodified)
    {
      whenselected(guim1, diktei)
      {
        Serial.print("inputted ");
        Serial.println(cgui->selitem->text);
      }
      cgui->selitem->deselect(bgcolor);
      cgui->selitem = nullptr;
      cgui->selindex = -1;
      inputmodified = false;
    }
    else
    {
      cgui->newselect();
    }
  }
  else if (readkey == '#')
  {
    cgui->selitem->deselect(bgcolor);
    cgui->selitem = nullptr;
    cgui->selindex = -1;
    inputmodified = false;
  }else if(readkey == 'A'){
    nextab();
  }
  else if (isdigit(readkey) && readkey != 0)
  {
    if (cgui->selitem != nullptr)
    {
      cgui->selitem->addchar(readkey);
    }
  }
  delay(5);
}

void nextab(){
  tabindex ++;
  if(tabindex > 3){
    tabindex = 1;
  }
  drawtab();
}

void drawtab(){
  tft.fillRect(0, 0, 480, 270, WHITE);

  tft.fillRect(0, 270, 480, 50, RED);
  unsigned short tabcols[] = {0, 0, 0};
  switch (tabindex)
  {
  case 1:
    tabcols[0] = GREEN;
    tabcols[1] = BLUE;
    tabcols[2] = BLUE;
    break;

  case 2:
    tabcols[0] = BLUE;
    tabcols[1] = GREEN;
    tabcols[2] = BLUE;
    break;

  case 3:
    tabcols[0] = BLUE;
    tabcols[1] = BLUE;
    tabcols[2] = GREEN;
    tft.drawRect(350, 50, 30, 150, RED); //BALKEN
    tft.drawRect(405, 50, 30, 150, RED);
    showencpos();
    break;
  
  default:
    break;
  }

  tft.fillRect(40, 275, 120, 40, tabcols[0]);
  tft.fillRect(180, 275, 120, 40, tabcols[1]);
  tft.fillRect(320, 275, 120, 40, tabcols[2]);

  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  
  tft.setCursor(65, 284);
  tft.print("MAIN");

  tft.setCursor(205, 284);
  tft.print("CONF");

  tft.setCursor(345, 284);
  tft.print("DIAG");

  cgui->draw();

}

void showencpos()
{
    tft.fillRect(351, 51, 28, 148, WHITE); //BALKEN
    tft.fillRect(406, 51, 28, 148, WHITE);

    tft.setTextSize(2);
    tft.setTextColor(RED);
    tft.setCursor(351, 210);
    tft.print(pos1);

    tft.setCursor(406, 210);
    tft.print(pos2);

    int h1 = map(pos1, 2000, 9000, 0, 150);
    int h2 = map(pos2, 2000, 9000, 0, 150);

    if (pos1 != 0)
    {
        tft.fillRect(350, 50 + h1, 30, 10, RED);
    }
    if (pos2 != 0)
    {
        tft.fillRect(405, 50 + h2, 30, 10, RED);
    }
}
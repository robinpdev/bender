#include "graphics.h"
#include "keypad.h"

const short bgcolor = BLUE;
const short inputboxcolor = GREEN;
const short inputselcolor = RED;

#include "gui.h"

label testlabel("label");
label inputlabel("input: ");

input i1;
input i2;
input i3;
input i4;

guitem *colitems[] = {&testlabel, &i1, &i2};
collumn col(colitems, sizeof(colitems) / 2);

guitem *items[] = {&i4, &col, &i3}; 
gui guim(items, sizeof(items) / 2); 



void setup(){
  // start communicatie met pc en due
  Serial.begin(115200);
  Serial1.begin(115200);

  // start scherm
  tft.begin();
  tft.setRotation(5);
  tft.fillScreen(WHITE);

  guim.draw();
}

void loop()
{
  char readkey = keypad.getKey();
  if (readkey == 'D'){
    guim.newselect();
  }else if(isdigit(readkey)){
    if(guim.selitem != nullptr){
      guim.selitem->addchar(readkey);
    }
  }
  delay(5);
}
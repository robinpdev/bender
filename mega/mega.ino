#include "graphics.h"
#include "keypad.h"

const short bgcolor = BLUE;

#include "gui.h"

void setup(){
  // start communicatie met pc en due
  Serial.begin(115200);
  Serial1.begin(115200);
  
  // start scherm
  tft.begin();
  tft.setRotation(5);
  tft.fillScreen(WHITE);

  label testlabel("label");
  label inputlabel("input: ");

  input testinput;
  input input2;

  
  guitem* colitems[] = {&testlabel, &testinput, &testlabel};
  collumn col(colitems, sizeof(colitems) / 2);

  guitem* items[] = {&testlabel, &col, &input2};
  
  gui guim(items, sizeof(items) / 2);
  guim.draw();
  
  delay(1000);
  guim.newselect();

  delay(1000);
  guim.newselect();

  delay(1000);
  guim.newselect();



}

void loop(){

}
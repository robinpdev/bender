#include "graphics.h"
#include "keypad.h"

#include "gui.h"

void setup(){
  // start communicatie met pc en due
  Serial.begin(115200);
  Serial1.begin(115200);
  
  // start scherm
  tft.begin();
  tft.setRotation(5);
  tft.fillScreen(WHITE);

  label testlabel("passed");
  label l2("running test");

  guitem* items[] = {&testlabel, &l2, &testlabel};
  
  gui guim(items, sizeof(items) / 2);
  guim.draw();

  delay(1000);
  l2.color = GREEN;
  guim.draw();

  delay(1000);
  l2.text = "passed";
  l2.update();

}

void loop(){

}
#include <Keypad.h> //library van keypad
const byte ROWS = 4; //vier rijen
const byte COLS = 4; //vier kolommen

char hexaKeys[ROWS][COLS] = {
  {'D','C','B','A'},
  {'#','9','6','3'},
  {'0','8','5','2'},
  {'*','7','4','1'}
};

//definiering van de pins waarop de keypad is aangesloten
//de pins van de keypad normaal aangesloten
//byte rowPins[ROWS] = {A0, A1, A2, A3}; 
//byte colPins[COLS] = {A4, A5, A6, A7}; 

//de pins van de keypad, omgekeerd, zoals in de case
byte rowPins[ROWS] = {A7, A6, A5, A4}; 
byte colPins[COLS] = {A3, A2, A1, A0}; 
//maak object keypad
Keypad keypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
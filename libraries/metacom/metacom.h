//metacom is a header library for communication between serial ports on 2 arduinos
//the main use of this is transferring variables form 1 board to another
#include <arduino.h>

void handlepacket(char var, String returned);

void sendchar(char var, char input) {
  cp.write('!');
  cp.write(var);
  cp.write(input);
  cp.write('#');
  //npackets ++;
  //nrpackets ++;
}

void sendstring(char var, String input) {
  const int len = input.length() + 1;
  cp.write('!');
  cp.write(var);
  char buf[len];
  input.toCharArray(buf, len);
  for (int n = 0; n <= len - 1; n++) {
    cp.write(buf[n]);
  }
  cp.write('#');
  //npackets ++;
  //nrpackets ++;
}

void sendint(char var, int input) {
  cp.write('!');
  cp.write(var);
  cp.print(String(input));
  cp.write('#');
}

template<typename T>
void send(char var, T input){
    cp.write('!');
    cp.write(var);
    cp.print(input);
    cp.write('#');
}

void send(char var){
  cp.write('!');
  cp.write(var);
  cp.write('#');
}

void updatepackets(){
    bool ran = false;
    while(cp.read() == '!' && cp.available() >= 2){ //TODO: research how to wait for incomplete packets the best
        //pakcet detected, reading in packet
        bool ran = true;
        String returned = "";
        char var = cp.read();
        while(cp.available()){
            char inchar = cp.read();
            if(inchar == '#'){
                break;
            }
            returned += inchar;
        }

        handlepacket(var, returned);
        Serial.println();
    }
}